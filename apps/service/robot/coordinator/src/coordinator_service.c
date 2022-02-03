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
  TurnOffLed(cfg->led_cfg->dd_led0);
  TurnOffLed(cfg->led_cfg->dd_led1);
  TurnOffLed(cfg->led_cfg->dd_led2);

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
    if (count % 25 == 0) ToggleLed(cfg->led_cfg->dd_led0);

    while (k_msgq_get(cfg->rcvr_srv->msgq_out, &cfg->rcvr_srv->receiver_data,
                      K_NO_WAIT) == 0) {
      printk("%3d %3d %3d %3d, %3d %3d %3d %3d\n",
             (int)(cfg->rcvr_srv->receiver_data.channels[0] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[1] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[2] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[3] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[4] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[5] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[6] * 100),
             (int)(cfg->rcvr_srv->receiver_data.channels[7] * 100));
      // reverse cmd
      float linear_x = cfg->rcvr_srv->receiver_data.channels[2];
      if (linear_x > 0) {
        linear_x = 1.0f - linear_x;
      } else {
        linear_x = -(1.0f + linear_x);
      }
      // add deadzone
      if (linear_x > -0.05 && linear_x < 0.05) linear_x = 0.0;
      cfg->desired_motion.linear.x = linear_x;
      //   SetMotorCmd(cmd, -cmd);
      //   srv->actr_srv.actuator_cmd.motors[0] =
      //       srv->rcvr_srv->receiver_data.channels[2];
      //   srv->actr_srv.actuator_cmd.motors[1] =
      //       srv->rcvr_srv->receiver_data.channels[2];
      //   printk("Setting command from main loop: %d\n",
      //          (int)(srv->rcvr_srv->receiver_data.channels[2] * 100));
      //   while (k_msgq_put(srv->actr_srv.msgq_in, &srv->actr_srv.actuator_cmd,
      //                     K_NO_WAIT) != 0) {
      //     k_msgq_purge(srv->actr_srv.msgq_in);
      //   }
    }

    // task timing
    ++count;
    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}