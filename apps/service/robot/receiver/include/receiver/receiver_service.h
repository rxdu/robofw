/*
 * receiver_service.h
 *
 * Created on: Jan 29, 2022 15:28
 * Description:
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

typedef enum { RCVR_SBUS = 0 } ReceiverType;

typedef struct {
  ReceiverType type;
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;
  
  void *rcvr_cfg;
  struct k_msgq *msgq;
} ReceiverServiceConf;

#define RECEIVER_CHANNEL_NUMBER 8
typedef struct {
  bool signal_lost;
  float channels[RECEIVER_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ReceiverData;

bool StartReceiverService(ReceiverServiceConf *cfg);

#endif /* RECEIVER_SERVICE_H */
