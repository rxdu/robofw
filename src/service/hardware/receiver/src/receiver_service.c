/*
 * receiver_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "receiver/receiver_service.h"

#include "encoder/encoder_service.h"
extern EncoderServiceDef encoder_srv;

void DumyServiceLoop(void *p1, void *p2, void *p3) {
//  ReceiverServiceDef *def = (ReceiverServiceDef *) p1;
  while (1) {
    k_msleep(20);
  }
}

bool StartReceiverService(ReceiverServiceDef *def) {
  // init hardware
  if (def->sconf.type == RCVR_SBUS) {
    SbusConf *sbus_cfg = (SbusConf * )(def->sconf.rcvr_cfg);

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

    printk("encoder device 1: %s, %s\n", encoder_srv.sconf.dd_encoders[0]->device->name,
           encoder_srv.sconf.dd_encoders[1]->device->name);

    // create and start thread
    def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                     K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                     DumyServiceLoop, def, NULL, NULL,
                                     def->tconf.priority, 0,
                                     Z_TIMEOUT_MS(def->tconf.delay_ms));
    k_thread_name_set(def->tconf.tid, "receiver_service");
  } else {
    printk("[ERROR] Unknown receiver type\n");
    return false;
  }

  printk("encoder device 2: %s, %s\n", encoder_srv.sconf.dd_encoders[0]->device->name,
         encoder_srv.sconf.dd_encoders[1]->device->name);

  return true;
}
