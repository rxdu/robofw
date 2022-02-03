/*
 * light_service.h
 *
 * Created on: Feb 01, 2022 15:43
 * Description:
 *
 * Service:
 *  - Config: hardware for radio receiver (e.g. UART for SBUS)
 *  - Input: N/A
 *  - Output: ReceiverData (msgq_out)
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef LIGHT_SERVICE_H
#define LIGHT_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/dio_interface.h"

typedef struct {
  DioDescriptor *dd_dio_front;
} LightConf;

#define LIGHT_NUMBER 2
typedef struct {
  bool lights[LIGHT_NUMBER];
} LightCmd;

typedef struct {
  // thread config
  k_tid_t tid;
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;
  uint32_t period_ms;

  // light config
  LightConf *light_cfg;

  // message queue for output
  struct k_msgq *msgq_in;
  LightCmd light_cmd;
} LightServiceConf;

bool StartLightService(LightServiceConf *cfg);

#endif /* LIGHT_SERVICE_H */
