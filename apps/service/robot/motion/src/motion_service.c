/*
 * motion_service.c
 *
 * Created on: Feb 03, 2022 11:48
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "motion/motion_service.h"

// K_MSGQ_DEFINE(receiver_data_queue, sizeof(ReceiverData), 1, 8);

static void MotionServiceLoop(void *p1, void *p2, void *p3);

bool StartMotionService(MotionServiceConf *cfg) {
  // init hardware
  if (cfg->type == MOT_TBOT) {
  }

  //   cfg->msgq_out = &receiver_data_queue;

  // create and start thread
  cfg->tid = k_thread_create(cfg->thread, cfg->stack, cfg->stack_size,
                             MotionServiceLoop, cfg, NULL, NULL, cfg->priority,
                             0, cfg->delay);

  return true;
}

void MotionServiceLoop(void *p1, void *p2, void *p3) {
  MotionServiceConf *cfg = (MotionServiceConf *)p1;
  while (1) {
    while (k_msgq_get(cfg->coord_srv->msgq_out, &cfg->coord_srv->desired_motion,
                      K_NO_WAIT) == 0) {
      printk("desired motion: %3d %3d %3d, %3d %3d %3d\n",
             (int)(cfg->coord_srv->desired_motion.linear.x * 100),
             (int)(cfg->coord_srv->desired_motion.linear.y * 100),
             (int)(cfg->coord_srv->desired_motion.linear.z * 100),
             (int)(cfg->coord_srv->desired_motion.angular.x * 100),
             (int)(cfg->coord_srv->desired_motion.angular.y * 100),
             (int)(cfg->coord_srv->desired_motion.angular.z * 100));

      //   if (linear_x > 0) {
      //     linear_x = 1.0f - linear_x;
      //   } else {
      //     linear_x = -(1.0f + linear_x);
      //   }
      //   // add deadzone
      //   if (linear_x > -0.05 && linear_x < 0.05) linear_x = 0.0;
    }

    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}
