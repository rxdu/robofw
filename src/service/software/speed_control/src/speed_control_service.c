/*
 * speed_control_service.c
 *
 * Created on 4/21/22 8:23 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "speed_control/speed_control_service.h"

K_THREAD_STACK_DEFINE(speed_control_service_stack, 1024);

_Noreturn static void SpeedServiceLoop(void *p1, void *p2, void *p3);

bool StartSpeedControlService(SpeedControlServiceDef *def) {
  if (def->sdata.desired_rpm_msgq == NULL) return false;
  def->interface.desired_rpm_msgq_in = def->sdata.desired_rpm_msgq;

  // sanity check
  if (def->dependencies.actuator_interface == NULL) {
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
  ActuatorCmd actuator_cmd;

  while (1) {
    if (k_msgq_get(def->interface.desired_rpm_msgq_in, &desired_rpm,
                   K_NO_WAIT) == 0) {
//      printk("desired rpm: %04d %04d\n", desired_rpm.motors[0],
//             desired_rpm.motors[1]);

      //      actuator_cmd.motors[0] = receiver_data.channels[2];
      //      actuator_cmd.motors[1] = receiver_data.channels[2];

      while (
          k_msgq_put(def->dependencies.actuator_interface->actuator_cmd_msgq_in,
                     &actuator_cmd, K_NO_WAIT) != 0) {
        k_msgq_purge(
            def->dependencies.actuator_interface->actuator_cmd_msgq_in);
      }
    }

    // task timing
    k_msleep(def->tconf.period_ms);
  }
}