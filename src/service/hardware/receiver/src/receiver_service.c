/*
 * receiver_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "receiver/receiver_service.h"

bool StartReceiverService(ReceiverServiceDef *def) {
  // init hardware
  if (def->sconf.type == RCVR_SBUS) {
    SbusConf *sbus_cfg = (SbusConf *) (def->sconf.rcvr_cfg);

    if (sbus_cfg->dd_dio_inv) {
      ConfigureDio(sbus_cfg->dd_dio_inv, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
      SetDio(sbus_cfg->dd_dio_inv, 1);
    }

    if (!InitSbus(sbus_cfg)) {
      printk("[ERROR] Failed to initialize Sbus\n");
      return false;
    }

    if (def->sdata.rc_data_msgq == NULL) return false;
    def->interface.rc_data_msgq_out = def->sdata.rc_data_msgq;

    // create and start thread
    def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                     K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                     SbusReceiverServiceLoop, def, NULL, NULL,
                                     def->tconf.priority, 0,
                                     Z_TIMEOUT_MS(def->tconf.delay_ms));
    k_thread_name_set(def->tconf.tid, "receiver_service");
  } else {
    printk("[ERROR] Unknown receiver type\n");
    return false;
  }

  return true;
}
