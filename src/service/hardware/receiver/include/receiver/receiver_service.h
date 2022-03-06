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

#include "interface/service.h"
#include "sbus_receiver.h"

typedef enum { RCVR_SBUS = 0, RCVR_PPM } ReceiverType;

#define RECEIVER_CHANNEL_NUMBER 8
typedef struct {
  bool signal_lost;
  float channels[RECEIVER_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ReceiverData;

// service related config
typedef struct {
  ReceiverType type;
  void *rcvr_cfg;
} ReceiverSrvConf;

typedef struct {
  struct k_msgq *rc_data_msgq;
  ReceiverData receiver_data;
} ReceiverSrvData;

struct ReceiverInterface {
  struct k_msgq *rc_data_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config/data
  ReceiverSrvConf sconf;
  ReceiverSrvData sdata;

  // interface
  struct ReceiverInterface interface;
} ReceiverServiceDef;

bool StartReceiverService(ReceiverServiceDef *def);

#endif /* RECEIVER_SERVICE_H */
