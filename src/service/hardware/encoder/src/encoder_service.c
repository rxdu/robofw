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

K_THREAD_STACK_DEFINE(encoder_service_stack, 1024);

_Noreturn static void EncoderServiceLoop(void *p1, void *p2, void *p3);

bool StartEncoderService(EncoderServiceDef *def) {
  if (def->sdata.encoder_rpm_msgq == NULL) return false;
  def->interface.rpm_msgq_out = def->sdata.encoder_rpm_msgq;

  // sanity check
  if (def->sconf.active_encoder_num == 0 || def->sconf.active_encoder_num > ENCODER_CHANNEL_NUMBER) {
    printk("Invalid active encoder number: %d\n", def->sconf.active_encoder_num);
    return false;
  }
  for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
    if (def->sconf.dd_encoders[i] == NULL) {
      printk("Encoder hardware descriptor not set properly\n");
      return false;
    }
  }

  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, encoder_service_stack,
                                   K_THREAD_STACK_SIZEOF(encoder_service_stack),
                                   EncoderServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0,
                                   Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "encoder_service");

  return true;
}

_Noreturn void EncoderServiceLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *) p1;

  bool is_counting_up[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER] = {0};
  uint16_t reading_error[ENCODER_CHANNEL_NUMBER] = {0};
//  int32_t rpm_estimate[ENCODER_CHANNEL_NUMBER] = {0};

  bool first_time = true;
  int64_t time_stamp = k_uptime_get();
  int64_t time_diff = 0;
  EstimatedSpeed speed_estimate;

  while (1) {
    for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
      encoder_reading[i] = GetEncoderCount(def->sconf.dd_encoders[i]);
      is_counting_up[i] = IsEncoderCountingUp(def->sconf.dd_encoders[i]);
    }

    if (first_time) {
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        encoder_prev_reading[i] = encoder_reading[i];
      }
      first_time = false;
    } else {
      time_diff = k_uptime_delta(&time_stamp);
      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
        if (is_counting_up[i]) {
          reading_error[i] = encoder_reading[i] - encoder_prev_reading[i];
        } else {
          reading_error[i] = encoder_prev_reading[i] - encoder_reading[i];
        }
        speed_estimate.rpms[i] = (int32_t) (reading_error[i]) * 60 * 1000 /
            time_diff / def->sconf.pulse_per_round[i];
        encoder_prev_reading[i] = encoder_reading[i];
      }
    }

//    printk("diff: %lld; left: (%d), %d, %d； right: (%d), %d, %d\n", time_diff,
//           is_counting_up[0], encoder_reading[0], speed_estimate.rpms[0],
//           is_counting_up[1], encoder_reading[1], speed_estimate.rpms[1]);

    while (k_msgq_put(def->interface.rpm_msgq_out, &speed_estimate, K_NO_WAIT) != 0) {
      k_msgq_purge(def->interface.rpm_msgq_out);
    }

//    printk("system: %d\n ", CONFIG_SYS_CLOCK_TICKS_PER_SEC);
    k_msleep(def->tconf.period_ms);
  }
}