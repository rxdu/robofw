/* 
 * messenger_service.c
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "messenger/messenger_service.h"

static void MessengerServiceLoop(void *p1, void *p2, void *p3);

bool StartMessengerService(MessengerServiceDef *def) {
  // uplink CAN to onboard computer
//  struct zcan_filter can_filter;
//  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
//  can_filter.rtr = CAN_DATAFRAME;
//  can_filter.rtr_mask = 1;
//  can_filter.id_mask = 0;
//  ConfigureCan(&hw.cans->descriptor[TBOT_CAN_UPLINK],
//               CAN_NORMAL_MODE, 500000, can_filter);
//  ConfigureCan(&hw.cans->descriptor[TBOT_CAN_DOWNLINK],
//               CAN_NORMAL_MODE, 500000, can_filter);

  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                   K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                   MessengerServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0,
                                   Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "messenger_service");

  return true;
}

void MessengerServiceLoop(void *p1, void *p2, void *p3) {

}