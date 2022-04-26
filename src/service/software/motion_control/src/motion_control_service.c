/*
 * motion_control_service.c
 *
 * Created on 4/21/22 9:17 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "motion_control/motion_control_service.h"

#include <math.h>

#include "speed_control/speed_control_service.h"

K_THREAD_STACK_DEFINE(motion_service_stack, 1024);

_Noreturn static void MotionServiceLoop(void *p1, void *p2, void *p3);

bool StartMotionService(MotionControlServiceDef *def) {
  if (def->sdata.desired_motion_msgq == NULL) return false;
  def->interface.desired_motion_msgq_in = def->sdata.desired_motion_msgq;

  if (def->sdata.control_feedback_msgq == NULL) return false;
  def->interface.control_feedback_msgq_out = def->sdata.control_feedback_msgq;

  // sanity check
  if (def->dependencies.coordinator_interface == NULL ||
      def->dependencies.speed_control_interface == NULL) {
    printk("Dependency not set properly\n");
    return false;
  }

  // create and start thread
  def->tconf.tid = k_thread_create(
      &def->tconf.thread, motion_service_stack,
      K_THREAD_STACK_SIZEOF(motion_service_stack), MotionServiceLoop, def, NULL,
      NULL, def->tconf.priority, 0, Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "motion_control_service");

  return true;
}

_Noreturn void MotionServiceLoop(void *p1, void *p2, void *p3) {
  MotionControlServiceDef *def = (MotionControlServiceDef *)p1;

  DesiredMotion desired_motion;
  DesiredRpm target_rpm;

  const float r = 0.0188;
  const float track = 0.185;
  const float gear_ratio = 30;

  while (1) {
    int64_t t0 = k_loop_start();

    if (k_msgq_get(def->interface.desired_motion_msgq_in, &desired_motion,
                   K_NO_WAIT) == 0) {
      // calculate desired left/right speed from desired motion
      float omega_l = 0, omega_r = 0;
      omega_l = (desired_motion.linear - desired_motion.angular * track) / r;
      omega_r = (desired_motion.linear + desired_motion.angular * track) / r;

      target_rpm.motors[0] = omega_l * 60 / (2 * M_PI);
      target_rpm.motors[1] = omega_r * 60 / (2 * M_PI);

      target_rpm.motors[1] = -target_rpm.motors[1];

      printk(
          "desired motion: %.4f, %.4f, w_L: %.4f, w_R: %.4f, desired rpm: %d, "
          "%d\n",
          desired_motion.linear, desired_motion.angular, omega_l, omega_r,
          target_rpm.motors[0], target_rpm.motors[1]);

      while (k_msgq_put(
                 def->dependencies.speed_control_interface->desired_rpm_msgq_in,
                 &target_rpm, K_NO_WAIT) != 0) {
        k_msgq_purge(
            def->dependencies.speed_control_interface->desired_rpm_msgq_in);
      }
    }

    k_msleep_until(def->tconf.period_ms, t0);
  }
}