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

#define GEAR_RATIO 30
#define MAX_RPM 550

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
      //   printk("desired motion: %3d %3d %3d, %3d %3d %3d\n",
      //          (int)(desired_motion.linear.x * 100),
      //          (int)(desired_motion.linear.y * 100),
      //          (int)(desired_motion.linear.z * 100),
      //          (int)(desired_motion.angular.x * 100),
      //          (int)(desired_motion.angular.y * 100),
      //          (int)(desired_motion.angular.z * 100));

      float linear_x = desired_motion.linear.x;
      float angular_z = desired_motion.angular.z;

      float wheel_radius = 0.02;
      float wheelbase = 0.5;

      // from linear_x, angular_z to motors[0], motors[1]
      //   float vel_left = (linear_x - angular_z * wheelbase) / wheel_radius *
      //   0.02; float vel_right =
      //       (linear_x + angular_z * wheelbase) / wheel_radius * 0.01;
      float vel_left = linear_x;
      float vel_right = linear_x;

      //   printk("---> calc: %03d %03d\n", (int)(vel_left * 100),
      //          (int)(vel_right * 100));

      ActuatorCmd actuator_cmd;
      actuator_cmd.motors[0] = vel_left;
      actuator_cmd.motors[1] = vel_right;

      while (k_msgq_put(cfg->actr_srv->msgq_in, &actuator_cmd, K_NO_WAIT) !=
             0) {
        k_msgq_purge(cfg->actr_srv->msgq_in);
      }
    }

    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}
