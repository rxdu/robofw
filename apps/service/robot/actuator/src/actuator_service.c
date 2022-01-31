/*
 * actuator_service.c
 *
 * Created on: Jan 31, 2022 22:42
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "actuator/actuator_service.h"

K_MSGQ_DEFINE(actuator_data_queue, sizeof(ActuatorCmd), 1, 8);

static void ActuatorServiceLoop(void *p1, void *p2, void *p3);

bool StartActuatorService(ActuatorServiceConf *cfg) {
  // init hardware
  if (cfg->type == ACT_TBOT_BRUSHED) {
    TbotBrushedMotorConf *motor_cfg =
        (TbotBrushedMotorConf *)(cfg->actuator_cfg);
    if (!InitTbotBrushedMotor(motor_cfg)) {
      printk("[ERROR] Failed to initialize Tbot brushed motor\n");
      return false;
    }
  }

  cfg->msgq_in = &actuator_data_queue;

  // create and start thread
  k_thread_create(cfg->thread, cfg->stack, cfg->stack_size, ActuatorServiceLoop,
                  cfg, NULL, NULL, cfg->priority, 0, cfg->delay);

  return true;
}

void ActuatorServiceLoop(void *p1, void *p2, void *p3) {
  ActuatorServiceConf *cfg = (ActuatorServiceConf *)p1;
  ActuatorType type = cfg->type;
  while (1) {
    if (type == ACT_TBOT_BRUSHED) {
      UpdateTbotBrushedMotor(p1);
    } else if (type == ACT_BRUSHLESS) {
      // process PPM
    }
  }
}