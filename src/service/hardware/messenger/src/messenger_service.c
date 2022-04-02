/* 
 * messenger_service.c
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "messenger/messenger_service.h"

#include "encoder/encoder_service.h"

K_THREAD_STACK_DEFINE(messenger_service_stack, 1024);

_Noreturn static void MessengerServiceLoop(void *p1, void *p2, void *p3);

bool StartMessengerService(MessengerServiceDef *def) {
  // initialize hardware
  struct zcan_filter can_filter;
  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  can_filter.rtr = CAN_DATAFRAME;
  can_filter.rtr_mask = 1;
  can_filter.id_mask = 0;
  ConfigureCan(def->sconf.dd_can,
               CAN_NORMAL_MODE, 1000000, can_filter);

//  if (def->sdata.desired_motion_msgq == NULL) return false;
//  def->interface.desired_motion_msgq_out = def->sdata.desired_motion_msgq;

  // sanity check
  if (def->sconf.dd_can == NULL) {
    printk("Messenger can descriptor not set properly\n");
    return false;
  }

  if (def->dependencies.receiver_interface == NULL ||
      def->dependencies.actuator_interface == NULL ||
      def->dependencies.encoder_interface == NULL) {
    printk("Dependency not set properly\n");
    return false;
  }

  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, messenger_service_stack,
                                   K_THREAD_STACK_SIZEOF(messenger_service_stack),
                                   MessengerServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0,
                                   Z_TIMEOUT_MS(def->tconf.delay_ms));
  k_thread_name_set(def->tconf.tid, "messenger_service");

  return true;
}

_Noreturn void MessengerServiceLoop(void *p1, void *p2, void *p3) {
  MessengerServiceDef *def = (MessengerServiceDef *) p1;

  EstimatedSpeed speed_estimate;

  while (1) {
//    printk("running messenger\n");
    while (k_msgq_get(def->dependencies.encoder_interface->rpm_msgq_out,
                      &speed_estimate, K_FOREVER) == 0) {
      printk("left: %d； right: %d\n", speed_estimate.rpms[0], speed_estimate.rpms[1]);
    }

    k_msleep(def->tconf.period_ms);
  }
}