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
#define PULSE_PER_ROUND 1320.f  // 11*30*4
#define MAX_PULSE_PER_MS 10     // 450 r/m * 1320 p/r / 60s/m = 9900 p/s

static bool is_counting_up[ENCODER_CHANNEL_NUMBER] = {true, true};

K_THREAD_STACK_DEFINE(encoder_service_main_stack, 1024);
K_THREAD_STACK_DEFINE(encoder_service_tim_stack, 512);

_Noreturn static void EncoderServiceTimerLoop(void *p1, void *p2, void *p3);
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
  def->main_tconf.tid = k_thread_create(
      &def->main_tconf.thread, encoder_service_main_stack,
      K_THREAD_STACK_SIZEOF(encoder_service_main_stack), EncoderServiceMainLoop,
      def, NULL, NULL, def->main_tconf.priority, 0,
      Z_TIMEOUT_MS(def->main_tconf.delay_ms));
  k_thread_name_set(def->main_tconf.tid, "encoder_service_main");

  def->tim_tconf.tid = k_thread_create(
      &def->tim_tconf.thread, encoder_service_tim_stack,
      K_THREAD_STACK_SIZEOF(encoder_service_tim_stack), EncoderServiceTimerLoop,
      def, NULL, NULL, def->tim_tconf.priority, 0,
      Z_TIMEOUT_MS(def->tim_tconf.delay_ms));
  k_thread_name_set(def->tim_tconf.tid, "encoder_service_timer");

  return true;
}

_Noreturn void EncoderServiceTimerLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *)p1;

  bool first_time = true;
  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t reading_error[ENCODER_CHANNEL_NUMBER] = {0};

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
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        // is_counting_up[i] = IsEncoderCountingUp(def->sconf.dd_encoders[i]);
        if (encoder_reading[i] >= encoder_prev_reading[i]) {
          reading_error[i] = encoder_reading[i] - encoder_prev_reading[i];
          if (reading_error[i] < MAX_PULSE_PER_MS) {
            is_counting_up[i] = true;
          } else {
            is_counting_up[i] = false;
          }
        } else {
          reading_error[i] = encoder_prev_reading[i] - encoder_reading[i];
          if (reading_error[i] < MAX_PULSE_PER_MS) {
            is_counting_up[i] = false;
          } else {
            is_counting_up[i] = true;
          }
        }

        encoder_prev_reading[i] = encoder_reading[i];
      }
    }
    k_msleep(def->tim_tconf.period_ms);
  }
}

_Noreturn void EncoderServiceMainLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *)p1;

  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t reading_error[ENCODER_CHANNEL_NUMBER] = {0};

  bool first_time = true;
  int64_t time_stamp = k_uptime_get();
  int64_t time_diff = 0;

  EstimatedSpeed speed_estimate;

  while (1) {
    for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
      encoder_reading[i] = GetEncoderCount(def->sconf.dd_encoders[i]);
      //   is_counting_up[i] = IsEncoderCountingUp(def->sconf.dd_encoders[i]);
    }

    if (first_time) {
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        encoder_prev_reading[i] = encoder_reading[i];
      }
      first_time = false;
    } else {
      time_diff = k_uptime_delta(&time_stamp);

      int32_t sign[ENCODER_CHANNEL_NUMBER] = {1, -1};
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        if (is_counting_up[i]) {
          reading_error[i] = encoder_reading[i] - encoder_prev_reading[i];
        } else {
          reading_error[i] = encoder_prev_reading[i] - encoder_reading[i];
          sign[i] = -1 * sign[i];
        }
        speed_estimate.rpms[i] = sign[i] * (int32_t)(reading_error[i]) * 60 *
                                 1000 / time_diff /
                                 def->sconf.pulse_per_round[i];
        encoder_prev_reading[i] = encoder_reading[i];
      }
    }

    printk("diff: %lld; left: (%s), %d, %d； right: (%s), %d, %d\n", time_diff,
           is_counting_up[0] ? "up" : "down", encoder_reading[0],
           speed_estimate.rpms[0], is_counting_up[1] ? "up" : "down",
           encoder_reading[1], speed_estimate.rpms[1]);

    while (k_msgq_put(def->interface.rpm_msgq_out, &speed_estimate,
                      K_NO_WAIT) != 0) {
      k_msgq_purge(def->interface.rpm_msgq_out);
    }

    //    printk("system: %d\n ", CONFIG_SYS_CLOCK_TICKS_PER_SEC);
    k_msleep(def->main_tconf.period_ms);
  }
}