/*
 * receiver_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "receiver/receiver_service.h"

K_MSGQ_DEFINE(receiver_data_queue, sizeof(ReceiverData), 1, 8);

static void ReceiverServiceLoop(void *p1, void *p2, void *p3);

bool StartReceiverService(ReceiverServiceConf *cfg) {
  // init hardware
  if (cfg->type == RCVR_SBUS) {
    SbusConf *sbus_cfg = (SbusConf *)(cfg->rcvr_cfg);
    if (!InitSbus(sbus_cfg)) {
      printk("[ERROR] Failed to initialize Sbus\n");
      return false;
    }
  }

  cfg->msgq = &receiver_data_queue;

  // create and start thread
  k_thread_create(cfg->thread, cfg->stack, cfg->stack_size, ReceiverServiceLoop,
                  cfg, NULL, NULL, cfg->priority, 0, cfg->delay);

  return true;
}

void ReceiverServiceLoop(void *p1, void *p2, void *p3) {
  ReceiverServiceConf *cfg = (ReceiverServiceConf *)p1;
  ReceiverType type = cfg->type;
  while (1) {
    if (type == RCVR_SBUS) {
      UpdateSbus(p1);
    } else if (type == RCVR_PPM) {
      // process PPM
    }
  }
}
