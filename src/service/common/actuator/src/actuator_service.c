/*
 * actuator_service.c
 *
 * Created on: Jan 31, 2022 22:42
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "actuator/actuator_service.h"
#include "actuator/tbot_actuators.h"
#include "actuator/rccar_actuators.h"

K_THREAD_STACK_DEFINE(actuator_service_stack, 1024);

bool StartActuatorService(ActuatorServiceDef *def) {
  // sanity check
  if (def->sdata.actuator_cmd_msgq == NULL) return false;
  def->interface.actuator_cmd_msgq_in = def->sdata.actuator_cmd_msgq;

  if (def->sconf.active_motor_num == 0 ||
      def->sconf.active_motor_num > ACTUATOR_CHANNEL_NUMBER) {
    printk("Invalid active actuator number: %d\n", def->sconf.active_motor_num);
    return false;
  }
  if (def->sconf.actuator_cfg == NULL) {
    printk("Empty actuator cfg\n");
    return false;
  }

  // init hardware
  if (def->sconf.type == ACTR_TBOT) {
    TbotActuatorConf *motor_cfg = (TbotActuatorConf *)(def->sconf.actuator_cfg);
    if (!InitTbotActuators(motor_cfg)) {
      printk("[ERROR] Failed to initialize Tbot actuator interface\n");
      return false;
    }

    // create and start thread
    def->tconf.tid = k_thread_create(
        &def->tconf.thread, actuator_service_stack,
        K_THREAD_STACK_SIZEOF(actuator_service_stack), TbotActuatorServiceLoop,
        def, NULL, NULL, def->tconf.priority, 0,
        Z_TIMEOUT_MS(def->tconf.delay_ms));
  } else if (def->sconf.type == ACTR_RCCAR) {
    RcCarActuatorConf *motor_cfg =
        (RcCarActuatorConf *)(def->sconf.actuator_cfg);
    if (!InitRcCarActuators(motor_cfg)) {
      printk("[ERROR] Failed to initialize RCCar actuator interface\n");
      return false;
    }

    // create and start thread
    def->tconf.tid = k_thread_create(
        &def->tconf.thread, actuator_service_stack,
        K_THREAD_STACK_SIZEOF(actuator_service_stack), RcCarActuatorServiceLoop,
        def, NULL, NULL, def->tconf.priority, 0,
        Z_TIMEOUT_MS(def->tconf.delay_ms));
  } else {
    return false;
  }

  k_thread_name_set(def->tconf.tid, "actuator_service");

  return true;
}
