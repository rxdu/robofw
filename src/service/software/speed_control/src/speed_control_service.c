/*
 * speed_control_service.c
 *
 * Created on 4/21/22 8:23 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "speed_control/speed_control_service.h"

#include "pid_controller.h"

K_THREAD_STACK_DEFINE(speed_control_service_stack, 1024);

_Noreturn static void SpeedServiceLoop(void *p1, void *p2, void *p3);

bool StartSpeedControlService(SpeedControlServiceDef *def) {
  if (def->sdata.desired_rpm_msgq == NULL) return false;
  def->interface.desired_rpm_msgq_in = def->sdata.desired_rpm_msgq;

  if (def->sdata.control_feedback_msgq == NULL) return false;
  def->interface.control_feedback_msgq_out = def->sdata.control_feedback_msgq;

  // sanity check
  if (def->dependencies.actuator_interface == NULL ||
      def->dependencies.encoder_interface == NULL) {
    printk("Dependency not set properly\n");
    return false;
  }

  // create and start thread
  def->tconf.tid = k_thread_create(
      &def->tconf.thread, speed_control_service_stack,
      K_THREAD_STACK_SIZEOF(speed_control_service_stack), SpeedServiceLoop, def,
      NULL, NULL, def->tconf.priority, 0, Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "speed_control_service");

  return true;
}

_Noreturn void SpeedServiceLoop(void *p1, void *p2, void *p3) {
  SpeedControlServiceDef *def = (SpeedControlServiceDef *)p1;

  DesiredRpm target_rpm;
  EstimatedSpeed measured_rpm;
  ActuatorCmd actuator_cmd;

  SpeedControlFeedback feedback;

  for (int i = 0; i < ACTUATOR_CHANNEL_NUMBER; ++i) {
    target_rpm.motors[i] = 0;
    measured_rpm.filtered_rpms[i] = 0;
    actuator_cmd.motors[i] = 0;
  }

  PidControllerInstance left_ctrl;
  left_ctrl.kp = 0.2;
  left_ctrl.ki = 0.5;
  left_ctrl.kd = 0;
  left_ctrl.umax = 100;
  left_ctrl.ts = def->tconf.period_ms / 1000.0f;
  InitPidController(&left_ctrl);

  PidControllerInstance right_ctrl;
  right_ctrl.kp = 0.2;
  right_ctrl.ki = 0.5;
  right_ctrl.kd = 0;
  right_ctrl.umax = 100;
  right_ctrl.ts = def->tconf.period_ms / 1000.0f;
  InitPidController(&right_ctrl);

  while (1) {
    int64_t t0 = k_loop_start();

    if (k_msgq_get(def->interface.desired_rpm_msgq_in, &target_rpm,
                   K_NO_WAIT) == 0) {
      //    printk("rpm cmd: %d, %d\n", desired_rpm.motors[0],
      //    desired_rpm.motors[1]);

      if (k_msgq_get(def->dependencies.encoder_interface->rpm_msgq_out,
                     &measured_rpm, K_NO_WAIT) == 0) {
        //      measured_rpm.filtered_rpms[1] = -measured_rpm.filtered_rpms[1];
      }

      actuator_cmd.motors[0] =
          UpdatePidController(&left_ctrl, target_rpm.motors[0],
                              measured_rpm.filtered_rpms[0]) /
          100.0f;
      actuator_cmd.motors[1] =
          UpdatePidController(&right_ctrl, target_rpm.motors[1],
                              measured_rpm.filtered_rpms[1]) /
          100.0f;

      //    printk("target/current: %d / %d, %d / %d, command: %f, %f\n",
      //           target_rpm.motors[0], measured_rpm.filtered_rpms[0],
      //           target_rpm.motors[1], measured_rpm.filtered_rpms[1],
      //           actuator_cmd.motors[0], actuator_cmd.motors[1]);

      while (
          k_msgq_put(def->dependencies.actuator_interface->actuator_cmd_msgq_in,
                     &actuator_cmd, K_NO_WAIT) != 0) {
        k_msgq_purge(
            def->dependencies.actuator_interface->actuator_cmd_msgq_in);
      }

      feedback.target_speed = target_rpm;
      feedback.measured_speed = measured_rpm;
      k_msgq_put(def->interface.control_feedback_msgq_out, &actuator_cmd,
                 K_NO_WAIT);
    }

    // task timing
    //    k_msleep(def->tconf.period_ms);
    k_msleep_until(def->tconf.period_ms, t0);
  }
}