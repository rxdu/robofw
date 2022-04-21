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
#include "speed_control/speed_control_service.h"

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
  ConfigureCan(def->sconf.dd_can, CAN_NORMAL_MODE, 500000, can_filter);

  //  if (def->sdata.desired_motion_msgq == NULL) return false;
  //  def->interface.desired_motion_msgq_out = def->sdata.desired_motion_msgq;

  // sanity check
  if (def->sconf.dd_can == NULL) {
    printk("Messenger can descriptor not set properly\n");
    return false;
  }

  if (def->dependencies.receiver_interface == NULL ||
      def->dependencies.actuator_interface == NULL ||
      def->dependencies.encoder_interface == NULL ||
      def->dependencies.speed_control_interface == NULL) {
    printk("Dependency not set properly\n");
    return false;
  }

  // create and start thread
  def->rx_tconf.tid = k_thread_create(
      &def->rx_tconf.thread, messenger_rx_service_stack,
      K_THREAD_STACK_SIZEOF(messenger_rx_service_stack), MessengerServiceRxLoop,
      def, NULL, NULL, def->rx_tconf.priority, 0,
      Z_TIMEOUT_MS(def->rx_tconf.delay_ms));
  k_thread_name_set(def->rx_tconf.tid, "messenger_rx_service");

  def->tx_tconf.tid = k_thread_create(
      &def->tx_tconf.thread, messenger_tx_service_stack,
      K_THREAD_STACK_SIZEOF(messenger_tx_service_stack), MessengerServiceTxLoop,
      def, NULL, NULL, def->tx_tconf.priority, 0,
      Z_TIMEOUT_MS(def->tx_tconf.delay_ms));
  k_thread_name_set(def->tx_tconf.tid, "messenger_tx_service");

  return true;
}

_Noreturn void MessengerServiceRxLoop(void *p1, void *p2, void *p3) {
  MessengerServiceDef *def = (MessengerServiceDef *)p1;
  struct zcan_frame rx_frame;

#ifndef UNKOWN_DECODER
  DecodeMsgType msg;
#endif

  ActuatorCmd actuator_cmd;
  DesiredRpm desired_rpm;

  while (1) {
    if (k_msgq_get(def->sconf.dd_can->msgq, &rx_frame, K_FOREVER) == 0) {
      //   printk("CAN1 %02x: ", rx_frame.id);
      //   for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ",
      //   rx_frame.data[i]); printk("\n");
#ifndef UNKOWN_DECODER
      if (DecodeCanMessage(&rx_frame, &msg)) {
        switch (msg.type) {
          case kTbotPwmCommand: {
            //            printk("pwm cmd: %d, %d\n", msg.data.pwm_cmd.pwm_left,
            //                   msg.data.pwm_cmd.pwm_right);
            actuator_cmd.motors[0] = msg.data.pwm_cmd.pwm_left / 100.0f;
            actuator_cmd.motors[1] = msg.data.pwm_cmd.pwm_right / 100.0f;
            while (
                k_msgq_put(
                    def->dependencies.actuator_interface->actuator_cmd_msgq_in,
                    &actuator_cmd, K_NO_WAIT) != 0) {
              k_msgq_purge(
                  def->dependencies.actuator_interface->actuator_cmd_msgq_in);
            }
            break;
          }
          case kTbotMotorCommand: {
            printk("rpm cmd: %d, %d\n", msg.data.rpm_cmd.rpm_left,
                   msg.data.rpm_cmd.rpm_right);
            desired_rpm.motors[0] = msg.data.rpm_cmd.rpm_left;
            desired_rpm.motors[1] = msg.data.rpm_cmd.rpm_right;
            while (k_msgq_put(def->dependencies.speed_control_interface
                                  ->desired_rpm_msgq_in,
                              &desired_rpm, K_NO_WAIT) != 0) {
              k_msgq_purge(def->dependencies.speed_control_interface
                               ->desired_rpm_msgq_in);
            }
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
  MessengerServiceDef *def = (MessengerServiceDef *)p1;

  EstimatedSpeed speed_estimate;

  TbotMsg tmsg;
  struct zcan_frame tx_frame;

  int ret = -1;
  (void)ret;

  while (1) {
    while (k_msgq_get(def->dependencies.encoder_interface->rpm_msgq_out,
                      &speed_estimate, K_FOREVER) == 0) {
      //   printk("left: %d； right: %d\n", speed_estimate.rpms[0],
      //          speed_estimate.rpms[1]);
      tmsg.type = kTbotEncoderRawData;
      tmsg.data.encoder_raw_data.left = speed_estimate.rpms[0];
      // invert value since motor is mechanically installed in an opposite
      // direction
      tmsg.data.encoder_raw_data.right = -speed_estimate.rpms[1];
      EncodeCanMessage(&tmsg, &tx_frame);

      ret = SendCanFrame(def->sconf.dd_can, tx_frame.id, true, tx_frame.data,
                         tx_frame.dlc);
      if (ret != CAN_TX_OK) {
        printk("%s send failed: %d\n", def->sconf.dd_can->device->name, ret);
      }

      tmsg.type = kTbotEncoderFilteredData;
      tmsg.data.encoder_filtered_data.left = speed_estimate.filtered_rpms[0];
      tmsg.data.encoder_filtered_data.right = -speed_estimate.filtered_rpms[1];
      EncodeCanMessage(&tmsg, &tx_frame);

      ret = SendCanFrame(def->sconf.dd_can, tx_frame.id, true, tx_frame.data,
                         tx_frame.dlc);
      if (ret != CAN_TX_OK) {
        printk("%s send failed: %d\n", def->sconf.dd_can->device->name, ret);
      }
    }
    k_msleep(def->tx_tconf.period_ms);
  }
}