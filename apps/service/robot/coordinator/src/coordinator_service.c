/*
 * coordinator_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "coordinator/coordinator_service.h"

K_MSGQ_DEFINE(desired_motion_queue, sizeof(DesiredMotion), 1, 8);

static void CoordinatorServiceLoop(void *p1, void *p2, void *p3);

bool StartCoordinatorService(CoordinatorServiceConf *cfg) {
  TurnOffLed(cfg->led_cfg->dd_led_status);

  cfg->msgq_out = &desired_motion_queue;

  // create and start thread
  cfg->tid = k_thread_create(cfg->thread, cfg->stack, cfg->stack_size,
                             CoordinatorServiceLoop, cfg, NULL, NULL,
                             cfg->priority, 0, cfg->delay);
  return true;
}

void CoordinatorServiceLoop(void *p1, void *p2, void *p3) {
  CoordinatorServiceConf *cfg = (CoordinatorServiceConf *)p1;
  uint8_t count = 0;
  while (1) {
    if (count % 25 == 0) ToggleLed(cfg->led_cfg->dd_led_status);

    while (k_msgq_get(cfg->rcvr_srv->msgq_out, &cfg->rcvr_srv->receiver_data,
                      K_NO_WAIT) == 0) {
      //   printk("%3d %3d %3d %3d, %3d %3d %3d %3d\n",
      //          (int)(cfg->rcvr_srv->receiver_data.channels[0] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[1] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[2] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[3] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[4] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[5] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[6] * 100),
      //          (int)(cfg->rcvr_srv->receiver_data.channels[7] * 100));
      cfg->desired_motion.linear.x = cfg->rcvr_srv->receiver_data.channels[2];
      cfg->desired_motion.angular.z = cfg->rcvr_srv->receiver_data.channels[3];
      while (k_msgq_put(cfg->msgq_out, &cfg->desired_motion, K_NO_WAIT) != 0) {
        k_msgq_purge(cfg->msgq_out);
      }
    }

    // task timing
    ++count;
    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}