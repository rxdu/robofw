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
#define ITERATION_NUM_PER_CALCULATION 4

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
      K_THREAD_STACK_SIZEOF(encoder_service_stack), EncoderServiceMainLoop,
      def, NULL, NULL, def->tconf.priority, 0,
      Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "encoder_service_main");

  return true;
}

_Noreturn void EncoderServiceMainLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *) p1;

  bool is_counting_up[ENCODER_CHANNEL_NUMBER] = {true, true};
  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t accumulated_error[ENCODER_CHANNEL_NUMBER] = {0};

  bool first_time = true;
  int64_t time_stamp = k_uptime_get();
  int64_t time_diff = 0;
  int64_t accumulated_time = 0;

  uint32_t loop_counter = 0;
  EstimatedSpeed speed_estimate;

  while (1) {
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
//            printk("overflow %d ----------------------------\n", i);
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
      if (loop_counter++ % ITERATION_NUM_PER_CALCULATION == 0) {
        for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
          int32_t sign = 1;
          if (!is_counting_up[i]) sign = -1;
          speed_estimate.rpms[i] = sign * (int32_t) (accumulated_error[i]) * 60 *
              1000 / accumulated_time /
              def->sconf.pulse_per_round[i];
        }
//        printk("period: %lld; left: (%s), %d, %d, %d； right: (%s), %d, %d, %d\n", accumulated_time,
//               is_counting_up[0] ? "up" : "down", encoder_reading[0], accumulated_error[0],
//               speed_estimate.rpms[0], is_counting_up[1] ? "up" : "down",
//               encoder_reading[1], accumulated_error[1], speed_estimate.rpms[1]);

        // clear time and error
        accumulated_time = 0;
        for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
          accumulated_error[i] = 0;
        }

        // write to queue
        while (k_msgq_put(def->interface.rpm_msgq_out, &speed_estimate,
                          K_NO_WAIT) != 0) {
          k_msgq_purge(def->interface.rpm_msgq_out);
        }
      }
    }

    k_msleep(def->tconf.period_ms);
  }
}