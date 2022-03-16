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

static void SpeedControlServiceLoop(void *p1, void *p2, void *p3);

K_MSGQ_DEFINE(control_data_queue, sizeof(DesiredSpeed), 1, 8);

bool StartSpeedControlService(SpeedControlServiceConf *cfg) {
  // create and start thread
  cfg->tid = k_thread_create(cfg->thread, cfg->stack, cfg->stack_size,
                             SpeedControlServiceLoop, cfg, NULL, NULL,
                             cfg->priority, 0, cfg->delay);
  return true;
}

void SpeedControlServiceLoop(void *p1, void *p2, void *p3) {
  SpeedControlServiceConf *cfg = (SpeedControlServiceConf *)p1;

  uint16_t encoder_reading[ACTUATOR_CHANNEL_NUMBER];
  bool is_counting_up[ACTUATOR_CHANNEL_NUMBER];
  uint16_t encoder_prev_reading[ACTUATOR_CHANNEL_NUMBER] = {0};

  uint16_t reading_error[ACTUATOR_CHANNEL_NUMBER] = {0};
  int32_t rpm_estimate[ACTUATOR_CHANNEL_NUMBER] = {0};

  bool first_time = true;

  while (1) {
    for (int i = 0; i < cfg->encoder_cfg->active_encoder_num; ++i) {
      encoder_reading[i] = GetEncoderCount(cfg->encoder_cfg->dd_encoders[i]);
      is_counting_up[i] = IsEncoderCountingUp(cfg->encoder_cfg->dd_encoders[i]);
    }

    if (first_time) {
      for (int i = 0; i < cfg->encoder_cfg->active_encoder_num; ++i) {
        encoder_prev_reading[i] = encoder_reading[i];
      }
      first_time = false;
    } else {
      for (int i = 0; i < cfg->encoder_cfg->active_encoder_num; ++i) {
        // if (is_counting_up[i])
        reading_error[i] = encoder_reading[i] - encoder_prev_reading[i];
        // else
        //   reading_error[i] = encoder_prev_reading[i] - encoder_reading[i];
        rpm_estimate[i] = (int32_t)(reading_error[i]) * 60 * 1000 /
                          cfg->period_ms / cfg->encoder_cfg->pulse_per_round[i];

        if (i == 0)
          printk("%s, current: %d, prev: %d, error: %d\n",
                 is_counting_up[i] ? "up" : "dn", encoder_reading[i],
                 encoder_prev_reading[i], reading_error[i]);

        encoder_prev_reading[i] = encoder_reading[i];
      }

      //   printk("left error: %lld, right error: %lld\n", reading_error[0],
      //          reading_error[1]);
      //   printk("left rpm: %d, right rpm: %d\n", rpm_estimate[0],
      //   rpm_estimate[1]);
    }

    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}