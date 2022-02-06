/*
 * speed_control_service.c
 *
 * Created on: Feb 06, 2022 16:34
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "speed_control/speed_control_service.h"

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

  uint32_t count1 = 0;
  bool is_up1 = false;
  uint32_t count2 = 0;
  bool is_up2 = false;
  uint32_t prev_count[2] = {0, 0};

  bool first_time = true;

  while (1) {
    count1 = GetEncoderCount(cfg->encoder_cfg->dd_encoders[0]);
    count2 = GetEncoderCount(cfg->encoder_cfg->dd_encoders[1]);

    is_up1 = IsEncoderCountingUp(cfg->encoder_cfg->dd_encoders[0]);
    is_up2 = IsEncoderCountingUp(cfg->encoder_cfg->dd_encoders[1]);

    if (first_time) {
      first_time = false;
      prev_count[0] = count1;
      prev_count[1] = count2;
    } else {
      uint32_t error1;
      if (is_up1) {
        error1 = count1 - prev_count[0];
      } else {
        error1 = prev_count[0] - count1;
      }
      uint32_t error2;
      if (is_up2) {
        error2 = count2 - prev_count[1];
      } else {
        error2 = prev_count[1] - count2;
      }

      prev_count[0] = count1;
      prev_count[1] = count2;

      float speed_left =
          error1 / PULSE_PER_ROUND * WHEEL_RADIUS * 2 * M_PI / EST_PERIOD;
      float speed_right =
          error2 / PULSE_PER_ROUND * WHEEL_RADIUS * 2 * M_PI / EST_PERIOD;

      printk("1: count %s: %d, 2: count %s: %d ; left: %f, right: %f\n",
             is_up1 ? "up" : "down", count1, is_up2 ? "up" : "down", count2,
             speed_left, speed_right);
    }

    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}