/*
 * messenger_service.c
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "messenger/messenger_service.h"

// #ifdef APP_tbot
// #include "tbot/tbot_messenger.h"
// typedef TbotMsg DecodeMsgType;
// #elseif defined(APP_tbot)
// #include "vesc/vesc_status_packet.hpp"
// #include "vesc/vesc_cmd_packet.hpp"
// #else
// #define UNKOWN_DECODER
// #endif

#include "vesc/vesc_cmd_parser.h"

K_THREAD_STACK_DEFINE(messenger_rx_service_stack, 512);
K_THREAD_STACK_DEFINE(messenger_tx_service_stack, 1024);

_Noreturn static void MessengerServiceRxLoop(void *p1, void *p2, void *p3);
_Noreturn static void MessengerServiceTxLoop(void *p1, void *p2, void *p3);

bool StartMessengerService(MessengerServiceDef *def) {
  // sanity check
  if (def->sconf.dd_can == NULL) {
    printk("Messenger can descriptor not set properly\n");
    return false;
  }

  if (def->sdata.robot_state_msgq == NULL) return false;
  def->interface.robot_state_msgq_in = def->sdata.robot_state_msgq;

  if (def->sdata.desired_motion_msgq == NULL) return false;
  def->interface.desired_motion_msgq_in = def->sdata.desired_motion_msgq;

  //   if (def->dependencies.actuator_interface == NULL ||
  //       def->dependencies.speed_control_interface == NULL) {
  //     printk("Dependency not set properly\n");
  //     return false;
  //   }

  // initialize hardware
  struct zcan_filter can_filter;
  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  can_filter.rtr = CAN_DATAFRAME;
  can_filter.rtr_mask = 1;
  can_filter.id_mask = 0;
  ConfigureCan(def->sconf.dd_can, CAN_NORMAL_MODE, 500000, can_filter);

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

  VescCmdPacket pkt;
  //   ActuatorCmd actuator_cmd;
  //   DesiredRpm desired_rpm;
  //   DesiredMotion desired_motion;

  while (1) {
    if (k_msgq_get(def->sconf.dd_can->msgq, &rx_frame, K_FOREVER) == 0) {
      //   printk("CAN1 %02x: ", rx_frame.id);
      //   for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ",
      //   rx_frame.data[i]); printk("\n");

      if (CanFrameToCmdPacket(&rx_frame, &pkt)) {
        switch (pkt.type) {
          case VescSetServoPosCmd: {
            //         //            printk("rpm cmd: %d, %d\n",
            //         msg.data.rpm_cmd.rpm_left,
            //             //                   msg.data.rpm_cmd.rpm_right);
            //             desired_rpm.motors[0] = msg.data.rpm_cmd.rpm_left;
            //         // invert value since motor is mechanically installed in
            //         an opposite
            //             // direction
            //             desired_rpm.motors[1] = -msg.data.rpm_cmd.rpm_right;
            //         while
            //         (k_msgq_put(def->dependencies.speed_control_interface
            //                               ->desired_rpm_msgq_in,
            //                           &desired_rpm, K_NO_WAIT) != 0) {
            //           k_msgq_purge(def->dependencies.speed_control_interface
            //                            ->desired_rpm_msgq_in);
            //         }
            break;
          }
          case VescSetRpmCmd: {
            //         desired_motion.linear = msg.data.motion_cmd.linear;
            //         desired_motion.angular = msg.data.motion_cmd.angular;
            //         while (k_msgq_put(def->interface.desired_motion_msgq_out,
            //                           &desired_motion, K_NO_WAIT) != 0) {
            //           k_msgq_purge(def->interface.desired_motion_msgq_out);
            //         }
            break;
          }
          default: {
            // do nothing
          }
        }
      }
    }
  }
}

_Noreturn void MessengerServiceTxLoop(void *p1, void *p2, void *p3) {
  MessengerServiceDef *def = (MessengerServiceDef *)p1;

  //   SpeedControlFeedback speed_control_feedback;
  //  EstimatedSpeed speed_estimate;

  struct zcan_frame tx_frame;

  RobotState robot_state;

  int ret = -1;
  (void)ret;

  while (1) {
    // while (k_msgq_get(def->dependencies.speed_control_interface
    //                       ->control_feedback_msgq_out,
    //                   &speed_control_feedback, K_NO_WAIT) == 0) {
    //   // encoder raw data
    //   tmsg.type = kTbotEncoderRawData;
    //   tmsg.data.encoder_raw_data.left =
    //       speed_control_feedback.measured_speed.raw_rpms[0];
    //   tmsg.data.encoder_raw_data.right =
    //       speed_control_feedback.measured_speed.raw_rpms[1];
    //   EncodeCanMessage(&tmsg, &tx_frame);

    //   ret = SendCanFrame(def->sconf.dd_can, tx_frame.id, true,
    //   tx_frame.data,
    //                      tx_frame.dlc);
    //   if (ret != CAN_TX_OK) {
    //     printk("%s send failed: %d\n", def->sconf.dd_can->device->name,
    //     ret);
    //   }
    // }

    // while (k_msgq_get(def->interface.robot_state_msgq_in, &robot_state,
    //                   K_NO_WAIT) == 0) {
    //   tmsg.type = kTbotSupervisedStateData;
    //   tmsg.data.supervised_state_data.sup_mode = robot_state.sup_mode;
    //   EncodeCanMessage(&tmsg, &tx_frame);

    //   ret = SendCanFrame(def->sconf.dd_can, tx_frame.id, true,
    //   tx_frame.data,
    //                      tx_frame.dlc);
    //   if (ret != CAN_TX_OK) {
    //     printk("%s send failed: %d\n", def->sconf.dd_can->device->name,
    //     ret);
    //   }
    // }

    k_msleep(def->tx_tconf.period_ms);
  }
}