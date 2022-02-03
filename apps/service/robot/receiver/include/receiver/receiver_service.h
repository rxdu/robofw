/*
 * receiver_service.h
 *
 * Created on: Jan 29, 2022 15:28
 * Description:
 *
 * Service:
 *  - Config: hardware for radio receiver (e.g. UART for SBUS)
 *  - Input: N/A
 *  - Output: ReceiverData (msgq_out)
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef RECEIVER_SERVICE_H
#define RECEIVER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "sbus_receiver.h"

typedef enum { RCVR_SBUS = 0, RCVR_PPM } ReceiverType;

#define RECEIVER_CHANNEL_NUMBER 8
typedef struct {
  bool signal_lost;
  float channels[RECEIVER_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ReceiverData;

typedef struct {
  // thread config
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;
  uint32_t period_ms;

  // receiver config
  ReceiverType type;
  void *rcvr_cfg;

  // message queue for output
  struct k_msgq *msgq_out;
  ReceiverData receiver_data;
} ReceiverServiceConf;

bool StartReceiverService(ReceiverServiceConf *cfg);

#endif /* RECEIVER_SERVICE_H */
