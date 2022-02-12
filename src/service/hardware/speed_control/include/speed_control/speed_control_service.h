/*
 * speed_control_service.h
 *
 * Created on: Feb 06, 2022 16:33
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef SPEED_CONTROL_SERVICE_H
#define SPEED_CONTROL_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/encoder_interface.h"
#include "actuator/actuator_service.h"

typedef struct {
  EncoderDescriptor *dd_encoders[ACTUATOR_CHANNEL_NUMBER];
} EncoderConfig;

typedef struct {
  float speed[ACTUATOR_CHANNEL_NUMBER];  // scaled to [-1, 1]
} DesiredSpeed;

typedef struct {
  // thread config
  k_tid_t tid;
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;
  uint32_t period_ms;

  // task-related config
  EncoderConfig *encoder_cfg;
  ActuatorServiceConf *actr_srv;

  // message queue for input/output
  struct k_msgq *msgq_in;
  DesiredSpeed control_data;
} SpeedControlServiceConf;

bool StartSpeedControlService(SpeedControlServiceConf *cfg);

#endif /* SPEED_CONTROL_SERVICE_H */