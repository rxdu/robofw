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
#include "robotconf/robotconf.h"

K_THREAD_STACK_DEFINE(messenger_rx_service_stack, 512);
K_THREAD_STACK_DEFINE(messenger_tx_service_stack, 1024);

static bool SendServoPosCmd(CanDescriptor *dd_can, float pos);
static bool SendSetDutyCycleCmd(CanDescriptor *dd_can, float duty);
static bool SendSetCurrentCmd(CanDescriptor *dd_can, float current);
static bool SendSetCurrentBrakeCmd(CanDescriptor *dd_can, float current);
static bool SendSetRpmCmd(CanDescriptor *dd_can, int32_t rpm);
static bool SendSetPositionCmd(CanDescriptor *dd_can, float pos);

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

  RobotState robot_state;
  DesiredMotion rc_desired_motion;

  while (1) {
    // check robot state to determine what command to send
    if (k_msgq_get(def->interface.robot_state_msgq_in, &robot_state,
                   K_NO_WAIT) == 0) {
      if (!robot_state.estop_triggered) {
        // manual control mode
        if (robot_state.control_mode == kControlModeRC) {
          if (k_msgq_get(def->interface.desired_motion_msgq_in,
                         &rc_desired_motion, K_NO_WAIT) == 0) {
            float servo_pos = (rc_desired_motion.angular + 1.0) / 2.0f;
            if (ENABLE_SERVO_INVERT) {
              servo_pos = 1.0 - servo_pos;
            }
            int32_t motor_rpm = VESC_MAX_RPM * rc_desired_motion.linear;
            printk("cmd: %f, %d\n", servo_pos, motor_rpm);
            SendServoPosCmd(def->sconf.dd_can, servo_pos);
            SendSetRpmCmd(def->sconf.dd_can, motor_rpm);
          }
        }
        // auto control mode (relay message to vesc)
        else if (robot_state.control_mode == kControlModeCAN) {
        }
      }
    }

    k_msleep(def->tx_tconf.period_ms);
  }
}

/******************************************************************************/

bool SendServoPosCmd(CanDescriptor *dd_can, float pos) {
  VescFrame tx_frame = VescSetServoPosCmdPacketToFrame(VESC_ID, pos);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}

bool SendSetDutyCycleCmd(CanDescriptor *dd_can, float duty) {
  VescFrame tx_frame = VescSetDutyCycleCmdPacketToFrame(VESC_ID, duty);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}

bool SendSetCurrentCmd(CanDescriptor *dd_can, float current) {
  VescFrame tx_frame = VescSetCurrentCmdPacketToFrame(VESC_ID, current);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}

bool SendSetCurrentBrakeCmd(CanDescriptor *dd_can, float current) {
  VescFrame tx_frame = VescSetCurrentBrakeCmdPacketToFrame(VESC_ID, current);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}

bool SendSetRpmCmd(CanDescriptor *dd_can, int32_t rpm) {
  VescFrame tx_frame = VescSetRpmCmdPacketToFrame(VESC_ID, rpm);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}

bool SendSetPositionCmd(CanDescriptor *dd_can, float pos) {
  VescFrame tx_frame = VescSetPositionCmdPacketToFrame(VESC_ID, pos);
  int ret =
      SendCanFrame(dd_can, tx_frame.id, false, tx_frame.data, tx_frame.dlc);
  if (ret != CAN_TX_OK) {
    printk("%s send failed: %d\n", dd_can->device->name, ret);
  }
  return ret == CAN_TX_OK;
}