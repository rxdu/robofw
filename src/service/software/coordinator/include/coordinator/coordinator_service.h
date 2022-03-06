/*
 * coordinator_service.h
 *
 * Created on: Jan 29, 2022 15:37
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef SYSTEM_SERVICE_H
#define SYSTEM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "receiver/receiver_service.h"
#include "mcal/interface/led_interface.h"

typedef struct {
  LedDescriptor *dd_led_status;
} LedConf;

typedef struct {
  float x;
  float y;
  float z;
} Vector3f;

typedef struct {
  Vector3f linear;
  Vector3f angular;
} DesiredMotion;

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
  LedConf *led_cfg;
  ReceiverServiceDef *rcvr_srv;

  // message queue for input/output
  struct k_msgq *msgq_out;
  DesiredMotion desired_motion;
} CoordinatorServiceConf;

bool StartCoordinatorService(CoordinatorServiceConf *cfg);

#endif /* SYSTEM_SERVICE_H */
