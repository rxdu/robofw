/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

#include "tbot/tbot_interface.h"
#include "receiver/receiver_service.h"

#define SLEEP_TIME_MS 50

#define TASK_PRIORITY_VIP -1  // Negative prio threads will not be pre-empted
#define TASK_PRIORITY_HIGHEST 1
#define TASK_PRIORITY_HIGH 2
#define TASK_PRIORITY_MID 3
#define TASK_PRIORITY_LOW 4

struct k_thread receiver_thread;
K_THREAD_STACK_DEFINE(receiver_service_stack, 1024);

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  RobotHardware *hw = GetHardware();

  if (!InitRobot()) {
    printk("[ERROR] Failed to initialize robot\n");
    while (true) {
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_STATUS]);
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_USER1]);
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_USER2]);
    }
  }

  SetMotorCmd(-0.5, -0.5);

  //   CanDescription *can_desc = GetCanDescription();

  ReceiverServiceConf rcvr_srv;
  rcvr_srv.type = RCVR_SBUS;
  rcvr_srv.priority = TASK_PRIORITY_HIGH;
  rcvr_srv.thread = &receiver_thread;
  rcvr_srv.stack = receiver_service_stack;
  rcvr_srv.stack_size = K_THREAD_STACK_SIZEOF(receiver_service_stack);
  rcvr_srv.delay = K_NO_WAIT;

  SbusConf sbus_cfg;
  sbus_cfg.dd = GetUartDescriptor(TBOT_UART_SBUS);

  rcvr_srv.rcvr_cfg = &sbus_cfg;

  bool ret = StartReceiverService(&rcvr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start receiver service\n");
  }

  printk("--------------------------------\n");

  ReceiverData rc_data;

  uint8_t count = 0;

  while (1) {
    if (count % 10 == 0) {
      ToggleLed(&hw->leds->descriptor[TBOT_LED_STATUS]);
    }

    while (k_msgq_get(rcvr_srv.msgq, &rc_data, K_NO_WAIT) == 0) {
      printk("%3d %3d %3d %3d, %3d %3d %3d %3d\n",
             (int)(rc_data.channels[0] * 100), (int)(rc_data.channels[1] * 100),
             (int)(rc_data.channels[2] * 100), (int)(rc_data.channels[3] * 100),
             (int)(rc_data.channels[4] * 100), (int)(rc_data.channels[5] * 100),
             (int)(rc_data.channels[6] * 100),
             (int)(rc_data.channels[7] * 100));
      float cmd = rc_data.channels[2];
      if (cmd > 0) {
        cmd = 1.0f - cmd;
      } else {
        cmd = -(1.0f + cmd);
      }
      if (cmd > -0.05 && cmd < 0.05) cmd = 0.0;
      SetMotorCmd(cmd, -cmd);
    }

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
