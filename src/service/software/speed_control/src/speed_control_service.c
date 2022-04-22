/*
 * speed_control_service.c
 *
 * Created on 4/21/22 8:23 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "speed_control/speed_control_service.h"
#include "encoder/encoder_service.h"

#include "pid_controller.h"

K_THREAD_STACK_DEFINE(speed_control_service_stack, 1024);

_Noreturn static void SpeedServiceLoop(void *p1, void *p2, void *p3);

bool StartSpeedControlService(SpeedControlServiceDef *def) {
  if (def->sdata.desired_rpm_msgq == NULL) return false;
  def->interface.desired_rpm_msgq_in = def->sdata.desired_rpm_msgq;

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

  DesiredRpm desired_rpm;
  EstimatedSpeed meas_rpm;
  ActuatorCmd actuator_cmd;

  for (int i = 0; i < ACTUATOR_CHANNEL_NUMBER; ++i) {
    desired_rpm.motors[i] = 0;
    meas_rpm.filtered_rpms[i] = 0;
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
    if (k_msgq_get(def->interface.desired_rpm_msgq_in, &desired_rpm,
                   K_NO_WAIT) == 0) {
    }

    //    printk("rpm cmd: %d, %d\n", desired_rpm.motors[0],
    //    desired_rpm.motors[1]);

    if (k_msgq_get(def->dependencies.encoder_interface->rpm_msgq_out, &meas_rpm,
                   K_NO_WAIT) == 0) {
    }

    actuator_cmd.motors[0] =
        UpdatePidController(&left_ctrl, desired_rpm.motors[0],
                            meas_rpm.filtered_rpms[0]) /
        100.0f;
    actuator_cmd.motors[1] =
        UpdatePidController(&right_ctrl, desired_rpm.motors[1],
                            meas_rpm.filtered_rpms[1]) /
        100.0f;

    printk("target: %d, %d, command: %f, %f\n", desired_rpm.motors[0],
           desired_rpm.motors[1], actuator_cmd.motors[0],
           actuator_cmd.motors[1]);

    //    while (
    //        k_msgq_put(def->dependencies.actuator_interface->actuator_cmd_msgq_in,
    //                   &actuator_cmd, K_NO_WAIT) != 0) {
    //      k_msgq_purge(def->dependencies.actuator_interface->actuator_cmd_msgq_in);
    //    }

    // task timing
    k_msleep(def->tconf.period_ms);
  }
}