/*
 * actuator_service.h
 *
 * Created on: Jan 31, 2022 12:54
 * Description:
 *
 * Service:
 *  - Config: hardware for actuator (e.g. DIO & PWM)
 *  - Input: ActuatorCmd (msgq_in)
 *  - Output: N/A
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef ACTUATOR_ACTUATOR_SERVICE_H
#define ACTUATOR_ACTUATOR_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "interface/service.h"

typedef enum { ACTR_TBOT = 0, ACTR_RCCAR } ActuatorType;

#define ACTUATOR_CHANNEL_NUMBER 2
typedef struct {
  float motors[ACTUATOR_CHANNEL_NUMBER];  // scaled to [-1, 1]
} __attribute__((aligned(8))) ActuatorCmd;

typedef struct {
  ActuatorType type;
  uint8_t active_motor_num;
  void *actuator_cfg;
} ActuatorSrvConf;

typedef struct {
  struct k_msgq *actuator_cmd_msgq;
} ActuatorSrvData;

struct ActuatorInterface {
  struct k_msgq *actuator_cmd_msgq_in;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  ActuatorSrvConf sconf;
  ActuatorSrvData sdata;

  // no dependency

  // interface
  struct ActuatorInterface interface;
} ActuatorServiceDef;

bool StartActuatorService(ActuatorServiceDef *def);

#endif /* ACTUATOR_ACTUATOR_SERVICE_H */
