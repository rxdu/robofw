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
    DesiredMotion desired_motion;
    while (k_msgq_get(cfg->coord_srv->msgq_out, &desired_motion, K_NO_WAIT) ==
           0) {
      printk("desired motion: %3d %3d %3d, %3d %3d %3d\n",
             (int)(desired_motion.linear.x * 100),
             (int)(desired_motion.linear.y * 100),
             (int)(desired_motion.linear.z * 100),
             (int)(desired_motion.angular.x * 100),
             (int)(desired_motion.angular.y * 100),
             (int)(desired_motion.angular.z * 100));

      float linear_x = desired_motion.linear.x;

      ActuatorCmd actuator_cmd;
      actuator_cmd.motors[0] = linear_x;

      while (k_msgq_put(cfg->actr_srv->msgq_in, &actuator_cmd, K_NO_WAIT) !=
             0) {
        k_msgq_purge(cfg->actr_srv->msgq_in);
      }
    }

    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}
