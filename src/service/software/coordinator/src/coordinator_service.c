/*
 * coordinator_service.c
 *
 * Created on: Jan 29, 2022 15:03
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "coordinator/coordinator_service.h"
#include "actuator/actuator_service.h"

static void CoordinatorServiceLoop(void *p1, void *p2, void *p3);

bool StartCoordinatorService(CoordinatorServiceDef *def) {
  TurnOffLed(def->sconf.dd_led_status);

  if (def->sdata.desired_motion_msgq == NULL) return false;
  def->interface.desired_motion_msgq_out = def->sdata.desired_motion_msgq;

  // create and start thread
  def->tconf.tid = k_thread_create(&def->tconf.thread, def->tconf.stack,
                                   K_THREAD_STACK_SIZEOF(def->tconf.stack),
                                   CoordinatorServiceLoop, def, NULL, NULL,
                                   def->tconf.priority, 0,
                                   Z_TIMEOUT_MS(def->tconf.delay_ms));
  return true;
}

void CoordinatorServiceLoop(void *p1, void *p2, void *p3) {
  CoordinatorServiceDef *def = (CoordinatorServiceDef *) p1;
  uint8_t count = 0;
  ActuatorCmd actuator_cmd;
  ReceiverData receiver_data;
  DesiredMotion desired_motion;

  printk("Running coordinator service\n");
  while (1) {
    if (count % 25 == 0) ToggleLed(def->sconf.dd_led_status);

//    printk("Checking receiver queue...\n");
    if (k_msgq_get(def->dependencies.receiver_interface->rc_data_msgq_out,
                   &receiver_data, K_NO_WAIT) == 0) {
      printk("coordinator: %04f %04f %04f %04f, %04f %04f %04f %04f <-----------\n",
             receiver_data.channels[0],
             receiver_data.channels[1],
             receiver_data.channels[2],
             receiver_data.channels[3],
             receiver_data.channels[4],
             receiver_data.channels[5],
             receiver_data.channels[6],
             receiver_data.channels[7]);
      printk("Throttle: %d ---------------\n", (int) (receiver_data.channels[2] * 100));

      actuator_cmd.motors[0] = receiver_data.channels[2];
      actuator_cmd.motors[1] = receiver_data.channels[2];

//      while (k_msgq_put(def->dependencies.actuator_interface->actuator_cmd_msgq_in,
//                        &actuator_cmd, K_NO_WAIT) != 0) {
//        k_msgq_purge(def->dependencies.actuator_interface->actuator_cmd_msgq_in);
//      }
    }

    // task timing
    ++count;
    k_msleep(def->tconf.period_ms);
  }
}