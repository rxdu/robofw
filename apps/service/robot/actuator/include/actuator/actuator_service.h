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

#ifndef ACTUATOR_SERVICE_H
#define ACTUATOR_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "actuator/tbot_actuators.h"

typedef enum { ACTR_TBOT = 0, ACTR_TA07PRO } ActuatorType;

#define ACTUATOR_CHANNEL_NUMBER 8
typedef struct {
  float motors[ACTUATOR_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ActuatorCmd;

typedef struct {
  // thread config
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;

  // receiver config
  ActuatorType type;
  uint8_t active_motor_num;
  void *actuator_cfg;

  // message queue for input
  struct k_msgq *msgq_in;
  ActuatorCmd actuator_cmd;
} ActuatorServiceConf;

bool StartActuatorService(ActuatorServiceConf *cfg);

#endif /* ACTUATOR_SERVICE_H */
