/*
 * encoder_service.c
 *
 * Created on: Feb 06, 2022 16:34
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "encoder/encoder_service.h"

#include <math.h>

#define WHEEL_RADIUS 0.0175f
#define EST_PERIOD 0.02
#define PULSE_PER_ROUND 1320.f    // 11*30*4
#define MAX_PULSE_PER_MS 10       // 450 r/m * 1320 p/r / 60s/m = 9900 p/s
#define MAX_PULSE_PER_PERIOD 150  // by experiment, in theory should be ~50
//#define ITERATION_NUM_PER_CALCULATION 4

#define MAX_POSSIBLE_RPM 600  // 560RPM + safety margin

#define AVERAGE_FILTER_LEN 10
#define MIN_ESTIMATE_NUM_BEFORE_VALID 20

static bool overflow_detected[ENCODER_CHANNEL_NUMBER] = {true};
static bool underflow_detected[ENCODER_CHANNEL_NUMBER] = {false};

K_THREAD_STACK_DEFINE(encoder_service_stack, 1024);

_Noreturn static void EncoderServiceMainLoop(void *p1, void *p2, void *p3);

bool StartEncoderService(EncoderServiceDef *def) {
  if (def->sdata.encoder_rpm_msgq == NULL) return false;
  def->interface.rpm_msgq_out = def->sdata.encoder_rpm_msgq;

  // sanity check
  if (def->sconf.active_encoder_num == 0 ||
      def->sconf.active_encoder_num > ENCODER_CHANNEL_NUMBER) {
    printk("Invalid active encoder number: %d\n",
           def->sconf.active_encoder_num);
    return false;
  }
  for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
    if (def->sconf.dd_encoders[i] == NULL) {
      printk("Encoder hardware descriptor not set properly\n");
      return false;
    }
  }

  // create and start thread
  def->tconf.tid = k_thread_create(
      &def->tconf.thread, encoder_service_stack,
      K_THREAD_STACK_SIZEOF(encoder_service_stack), EncoderServiceMainLoop, def,
      NULL, NULL, def->tconf.priority, 0, Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "encoder_service_main");

  return true;
}

_Noreturn void EncoderServiceMainLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *)p1;

  // low-pass filter
  static float a = 0.7284895;
  static float b[2] = {0.13575525, 0.13575525};
  int32_t last_filtered_rpms[ENCODER_CHANNEL_NUMBER] = {0, 0};
  int32_t raw_rpm_history[ENCODER_CHANNEL_NUMBER][2] = {{0, 0}, {0, 0}};

  // average filter
  uint8_t indexer[ENCODER_CHANNEL_NUMBER] = {0};
  int32_t estimated_rpm_history[ENCODER_CHANNEL_NUMBER][AVERAGE_FILTER_LEN] = {
      0};

  // encoder readings
  bool is_counting_up[ENCODER_CHANNEL_NUMBER] = {true, true};
  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER] = {0, 0};
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER] = {0, 0};
  uint16_t accumulated_error[ENCODER_CHANNEL_NUMBER] = {0, 0};

  bool first_time = true;
  int64_t time_stamp = k_uptime_get();
  int64_t time_diff = 0;
  int64_t accumulated_time = 0;

  uint32_t loop_counter = 0;
  uint32_t estimator_counter = 0;
  EstimatedSpeed speed_estimate;

  while (1) {
    int64_t t0 = k_loop_start();

    for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
      encoder_reading[i] = GetEncoderCount(def->sconf.dd_encoders[i]);
    }

    if (first_time) {
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        encoder_prev_reading[i] = encoder_reading[i];
      }
      first_time = false;
    } else {
      time_diff = k_uptime_delta(&time_stamp);
      accumulated_time += time_diff;

      // check overflow/underflow
      // positive direction: current > previous, unless overflow
      // negative direction: current < previous, unless underflow
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        if ((encoder_reading[i] >= encoder_prev_reading[i]) &&
            (encoder_reading[i] - encoder_prev_reading[i] >
             MAX_PULSE_PER_PERIOD)) {
          underflow_detected[i] = true;
          //          printk("underflow %d ----------------------------\n", i);
        } else {
          if (encoder_prev_reading[i] - encoder_reading[i] >
              MAX_PULSE_PER_PERIOD) {
            overflow_detected[i] = true;
            //            printk("overflow %d ----------------------------\n",
            //            i);
          }
        }

        // calculate rpm
        if (encoder_reading[i] >= encoder_prev_reading[i]) {
          if (underflow_detected[i]) {
            is_counting_up[i] = false;
            underflow_detected[i] = false;
          } else {
            is_counting_up[i] = true;
          }
        } else {
          if (overflow_detected[i]) {
            is_counting_up[i] = true;
            overflow_detected[i] = false;
          } else {
            is_counting_up[i] = false;
          }
        }

        // save error values
        uint16_t reading_error = 0;
        if (is_counting_up[i]) {
          reading_error = encoder_reading[i] - encoder_prev_reading[i];
        } else {
          reading_error = encoder_prev_reading[i] - encoder_reading[i];
        }
        accumulated_error[i] += reading_error;

        // update previous reading
        encoder_prev_reading[i] = encoder_reading[i];
      }

      // calculate rpm every ~20ms
      if (loop_counter++ % def->sconf.sample_per_every_iteration == 0) {
        for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
          int32_t sign = 1;
          if (!is_counting_up[i]) sign = -1;
          speed_estimate.raw_rpms[i] = sign * (int32_t)(accumulated_error[i]) *
                                       60 * 1000 / accumulated_time /
                                       def->sconf.pulse_per_round[i];

          // apply low-pass filter
          speed_estimate.filtered_rpms[i] = a * last_filtered_rpms[i] +
                                            b[0] * raw_rpm_history[i][1] +
                                            b[1] * raw_rpm_history[i][0];

          // remove outlier, if found, use previous estimated value
          if (speed_estimate.filtered_rpms[i] > MAX_POSSIBLE_RPM ||
              speed_estimate.filtered_rpms[i] < -MAX_POSSIBLE_RPM)
            speed_estimate.filtered_rpms[i] = last_filtered_rpms[i];

          // save for next iternation
          raw_rpm_history[i][0] = raw_rpm_history[i][1];
          raw_rpm_history[i][1] = speed_estimate.raw_rpms[i];
          last_filtered_rpms[i] = speed_estimate.filtered_rpms[i];

          // calculate average filtered rpm
          estimated_rpm_history[i][indexer[i]] =
              speed_estimate.filtered_rpms[i];
          indexer[i] = (indexer[i] + 1) % AVERAGE_FILTER_LEN;
          if (estimator_counter >= AVERAGE_FILTER_LEN) {
            int64_t sum = 0;
            for (int j = 0; j < AVERAGE_FILTER_LEN; ++j) {
              sum += estimated_rpm_history[i][j];
            }
            speed_estimate.filtered_rpms[i] = sum / AVERAGE_FILTER_LEN;
          }
        }

        // printk("left: %d, %d; right %d, %d\n",
        // speed_estimate.filtered_rpms[0],
        //        indexer[0], speed_estimate.filtered_rpms[1], indexer[1]);

        // increase counter as one estimate has been made
        estimator_counter++;

        // clear time and error
        accumulated_time = 0;
        for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
          accumulated_error[i] = 0;
        }

        // write to queue, only after at least MIN_ESTIMATE_NUM_BEFORE_VALID
        // estimates have been made
        if (estimator_counter > MIN_ESTIMATE_NUM_BEFORE_VALID) {
          // to avoid estimator_counter overflow
          estimator_counter = MIN_ESTIMATE_NUM_BEFORE_VALID;
          while (k_msgq_put(def->interface.rpm_msgq_out, &speed_estimate,
                            K_NO_WAIT) != 0) {
            k_msgq_purge(def->interface.rpm_msgq_out);
          }
        }
      }
    }

    k_msleep_until(def->tconf.period_ms, t0);
  }
}