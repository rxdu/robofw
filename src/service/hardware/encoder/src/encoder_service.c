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

_Noreturn static void EncoderServiceLoop(void *p1, void *p2, void *p3);

bool StartEncoderService(EncoderServiceDef *def) {
  if (def->sdata.encoder_rpm_msgq == NULL) return false;
  def->interface.estimated_rpm_msgq_out = def->sdata.encoder_rpm_msgq;

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
  def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                   K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                   EncoderServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0,
                                   Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "encoder_service");

  return true;
}

_Noreturn void EncoderServiceLoop(void *p1, void *p2, void *p3) {
  EncoderServiceDef *def = (EncoderServiceDef *) p1;

  uint16_t encoder_reading[ENCODER_CHANNEL_NUMBER];
  bool is_counting_up[ENCODER_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ENCODER_CHANNEL_NUMBER] = {0};

  uint16_t reading_error[ENCODER_CHANNEL_NUMBER] = {0};
  int32_t rpm_estimate[ENCODER_CHANNEL_NUMBER] = {0};

  bool first_time = true;

  while (1) {
    printk("encoder service running\n");
    if (def->sconf.dd_encoders[0] == NULL) printk("Null ptr 0 found\n");
    if (def->sconf.dd_encoders[1] == NULL) printk("Null ptr 1 found\n");

    printk("encoder device: %s, %s\n", def->sconf.dd_encoders[0]->device->name,
           def->sconf.dd_encoders[1]->device->name);

    printk("encoder count: %d, %d\n", GetEncoderCount(def->sconf.dd_encoders[0]),
           GetEncoderCount(def->sconf.dd_encoders[1]));
//    for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
//      encoder_reading[i] = GetEncoderCount(def->sconf.dd_encoders[i]);
//      is_counting_up[i] = IsEncoderCountingUp(def->sconf.dd_encoders[i]);
//    }

//    if (first_time) {
//      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
//        encoder_prev_reading[i] = encoder_reading[i];
//      }
//      first_time = false;
//    }
//    else {
//      for (int i = 0; i < def->sconf.active_encoder_num; ++i) {
//        // if (is_counting_up[i])
//        reading_error[i] = encoder_reading[i] - encoder_prev_reading[i];
//        // else
//        //   reading_error[i] = encoder_prev_reading[i] - encoder_reading[i];
//        rpm_estimate[i] = (int32_t) (reading_error[i]) * 60 * 1000 /
//            def->tconf.period_ms / def->sconf.pulse_per_round[i];
//
//        if (i == 0)
//          printk("%s, current: %d, prev: %d, error: %d\n",
//                 is_counting_up[i] ? "up" : "dn", encoder_reading[i],
//                 encoder_prev_reading[i], reading_error[i]);
//
//        encoder_prev_reading[i] = encoder_reading[i];
//      }
//
//      //   printk("left error: %lld, right error: %lld\n", reading_error[0],
//      //          reading_error[1]);
//      printk("left rpm: %d, right rpm: %d\n", rpm_estimate[0],
//             rpm_estimate[1]);
//    }

    k_msleep(def->tconf.period_ms);
  }
}