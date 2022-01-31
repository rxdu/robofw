/*
 * actuator_service.h
 *
 * Created on: Jan 31, 2022 12:54
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef ACTUATOR_SERVICE_H
#define ACTUATOR_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "actuator/tbot_brushed_motor.h"

typedef enum { ACT_TBOT_BRUSHED = 0, ACT_BRUSHLESS } ActuatorType;

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
} ActuatorServiceConf;

#define ACTUATOR_CHANNEL_NUMBER 8
typedef struct {
  float motors[ACTUATOR_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ActuatorCmd;

bool StartActuatorService(ActuatorServiceConf *cfg);

#endif /* ACTUATOR_SERVICE_H */
