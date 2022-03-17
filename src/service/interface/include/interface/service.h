/*
 * service.h
 *
 * Created on: Mar 06, 2022 14:16
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef SERVICE_H
#define SERVICE_H

#include <zephyr.h>
#include <device.h>

typedef struct {
  k_tid_t tid;
  struct k_thread thread;
  k_thread_stack_t *stack;
  int8_t priority;
  uint32_t delay_ms;
  uint32_t period_ms;
} ThreadConfig;

#endif /* SERVICE_H */
