/*
 * receiver_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "receiver/receiver_service.h"

// K_MSGQ_DEFINE(receiver_data_queue, sizeof(ReceiverData), 1, 8);

static void ReceiverServiceLoop(void *p1, void *p2, void *p3);

bool StartReceiverService(ReceiverServiceDef *def) {
  // init hardware
  if (def->sconf.type == RCVR_SBUS) {
    SbusConf *sbus_cfg = (SbusConf *)(def->sconf.rcvr_cfg);

    if (sbus_cfg->dd_dio_inv) {
      ConfigureDio(sbus_cfg->dd_dio_inv, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
      SetDio(sbus_cfg->dd_dio_inv, 1);
    }

    if (!InitSbus(sbus_cfg)) {
      printk("[ERROR] Failed to initialize Sbus\n");
      return false;
    }
  }

  def->interface.rc_data_msgq_out = def->sdata.rc_data_msgq;

  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                   K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                   ReceiverServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0, def->tconf.delay);

  return true;
}

void ReceiverServiceLoop(void *p1, void *p2, void *p3) {
  ReceiverServiceDef *def = (ReceiverServiceDef *)p1;
  ReceiverType type = def->sconf.type;
  while (1) {
    if (type == RCVR_SBUS) {
      UpdateSbus(p1);
    } else if (type == RCVR_PPM) {
      // process PPM
    }
    if (def->tconf.period_ms > 0) k_msleep(def->tconf.period_ms);
  }
}
