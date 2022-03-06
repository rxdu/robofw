/*
 * motion_service.h
 *
 * Created on: Feb 03, 2022 11:42
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef MOTION_SERVICE_H
#define MOTION_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "coordinator/coordinator_service.h"
#include "actuator/actuator_service.h"

typedef enum { MOT_TBOT = 0 } MotionType;

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
  MotionType type;
  CoordinatorServiceDef *coord_srv;
  ActuatorServiceConf *actr_srv;

  // message queue for input/output
  //   struct k_msgq *msgq_out;
  //   MotionData receiver_data;
} MotionServiceConf;

bool StartMotionService(MotionServiceConf *cfg);

#endif /* MOTION_SERVICE_H */
