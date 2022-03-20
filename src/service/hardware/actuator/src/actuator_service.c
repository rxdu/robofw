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

bool StartActuatorService(ActuatorServiceDef *def) {
  // init hardware
  if (def->sconf.type == ACTR_TBOT) {
    TbotActuatorConf *motor_cfg = (TbotActuatorConf *) (def->sconf.actuator_cfg);
    if (!InitTbotActuators(motor_cfg)) {
      printk("[ERROR] Failed to initialize Tbot brushed motor\n");
      return false;
    }

    if (def->sdata.actuator_cmd_msgq == NULL) return false;
    def->interface.actuator_cmd_msgq_in = def->sdata.actuator_cmd_msgq;

    // create and start thread
    def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                     K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                     TbotActuatorServiceLoop, def, NULL, NULL,
                                     def->tconf.priority, 0,
                                     Z_TIMEOUT_MS(def->tconf.delay_ms));
  }

  return true;
}
