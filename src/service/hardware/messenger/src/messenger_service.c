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
#include "actuator/actuator_service.h"

#ifdef APP_tbot
#include "tbot/tbot_messenger.h"
typedef TbotMsg DecodeMsgType;
#else
#define UNKOWN_DECODER
#endif

K_THREAD_STACK_DEFINE(messenger_rx_service_stack, 512);
K_THREAD_STACK_DEFINE(messenger_tx_service_stack, 1024);

_Noreturn static void MessengerServiceRxLoop(void *p1, void *p2, void *p3);
_Noreturn static void MessengerServiceTxLoop(void *p1, void *p2, void *p3);

bool StartMessengerService(MessengerServiceDef *def) {
  // initialize hardware
  struct zcan_filter can_filter;
  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  can_filter.rtr = CAN_DATAFRAME;
  can_filter.rtr_mask = 1;
  can_filter.id_mask = 0;
  ConfigureCan(def->sconf.dd_can,
               CAN_NORMAL_MODE, 500000, can_filter);

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
  def->rx_tconf.tid = k_thread_create(&def->rx_tconf.thread, messenger_rx_service_stack,
                                      K_THREAD_STACK_SIZEOF(messenger_rx_service_stack),
                                      MessengerServiceRxLoop, def, NULL, NULL,
                                      def->rx_tconf.priority, 0,
                                      Z_TIMEOUT_MS(def->rx_tconf.delay_ms));
  k_thread_name_set(def->rx_tconf.tid, "messenger_rx_service");

  def->tx_tconf.tid = k_thread_create(&def->tx_tconf.thread, messenger_tx_service_stack,
                                      K_THREAD_STACK_SIZEOF(messenger_tx_service_stack),
                                      MessengerServiceTxLoop, def, NULL, NULL,
                                      def->tx_tconf.priority, 0,
                                      Z_TIMEOUT_MS(def->tx_tconf.delay_ms));
  k_thread_name_set(def->tx_tconf.tid, "messenger_tx_service");

  return true;
}

_Noreturn void MessengerServiceRxLoop(void *p1, void *p2, void *p3) {
  MessengerServiceDef *def = (MessengerServiceDef *) p1;
  struct zcan_frame rx_frame;
#ifndef UNKOWN_DECODER
  TbotMsg msg;
#endif

  ActuatorCmd actuator_cmd;

  while (1) {
    if (k_msgq_get(def->sconf.dd_can->msgq, &rx_frame, K_FOREVER) == 0) {
      printk("CAN1 %02x: ", rx_frame.id);
      for (int i = 0; i < rx_frame.dlc; ++i)
        printk("%02x ",
               rx_frame.data[i]);
      printk("\n");
#ifndef UNKOWN_DECODER
      if (DecodeCanMessage(&rx_frame, &msg)) {
        switch (msg.type) {
          case kTbotPwmCommand: {
            printk("cmd: %d, %d\n", msg.data.pwm_cmd.pwm_left, msg.data.pwm_cmd.pwm_right);
            actuator_cmd.motors[0] = msg.data.pwm_cmd.pwm_left / 100.0f;
            actuator_cmd.motors[1] = msg.data.pwm_cmd.pwm_right / 100.0f;
            while (k_msgq_put(def->dependencies.actuator_interface->actuator_cmd_msgq_in,
                              &actuator_cmd, K_NO_WAIT) != 0) {
              k_msgq_purge(def->dependencies.actuator_interface->actuator_cmd_msgq_in);
            }
            break;
          }
          case kTbotMotorCommand: {
            break;
          }
          case kTbotMotionCommand: {
            break;
          }
        }
      }
#endif
    }
  }
}

_Noreturn void MessengerServiceTxLoop(void *p1, void *p2, void *p3) {
  MessengerServiceDef *def = (MessengerServiceDef *) p1;

  EstimatedSpeed speed_estimate;
  uint8_t candata[] = {0x11, 0x22, 0x55, 0x66};

  while (1) {
//    printk("running messenger\n");
//    while (k_msgq_get(def->dependencies.encoder_interface->rpm_msgq_out,
//                      &speed_estimate, K_FOREVER) == 0) {
//      printk("left: %d； right: %d\n", speed_estimate.rpms[0], speed_estimate.rpms[1]);
//    }

    int ret = SendCanFrame(def->sconf.dd_can, 0x121, true, candata, 4);
    if (ret != CAN_TX_OK) {
      printk("%s send failed: %d\n", def->sconf.dd_can->device->name, ret);
    } else {
//      printk("%s sent\n", def->sconf.dd_can->device->name);
    }

    k_msleep(def->tx_tconf.period_ms);
  }
}