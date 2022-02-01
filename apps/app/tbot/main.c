/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

#include "tbot_interface.h"

#define SLEEP_TIME_MS 50

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  RobotHardware *hw = GetHardware();
  RobotService *srv = GetService();

  if (!InitRobot()) {
    printk("[ERROR] Failed to initialize robot\n");
    while (true) {
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_STATUS]);
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_USER1]);
      TurnOnLed(&hw->leds->descriptor[TBOT_LED_USER2]);
    }
  }

  uint8_t count = 0;

  while (1) {
    if (count % 10 == 0) {
      ToggleLed(&hw->leds->descriptor[TBOT_LED_STATUS]);
    }

    while (k_msgq_get(srv->rcvr_srv.msgq_out, &srv->rcvr_srv.receiver_data,
                      K_NO_WAIT) == 0) {
      //   printk("%3d %3d %3d %3d, %3d %3d %3d %3d\n",
      //          (int)(rc_data.channels[0] * 100), (int)(rc_data.channels[1]
      //   *
      //          100), (int)(rc_data.channels[2] * 100),
      //          (int)(rc_data.channels[3] * 100), (int)(rc_data.channels[4]
      //   *
      //          100), (int)(rc_data.channels[5] * 100),
      //          (int)(rc_data.channels[6] * 100),
      //          (int)(rc_data.channels[7] * 100));
      //   float cmd = rc_data.channels[2];
      //   if (cmd > 0) {
      //     cmd = 1.0f - cmd;
      //   } else {
      //     cmd = -(1.0f + cmd);
      //   }
      //   if (cmd > -0.05 && cmd < 0.05) cmd = 0.0;
      //   SetMotorCmd(cmd, -cmd);
      srv->actr_srv.actuator_cmd.motors[0] =
          srv->rcvr_srv.receiver_data.channels[2];
      srv->actr_srv.actuator_cmd.motors[1] =
          srv->rcvr_srv.receiver_data.channels[2];
      printk("Setting command from main loop: %d\n",
             (int)(srv->rcvr_srv.receiver_data.channels[2] * 100));
      while (k_msgq_put(srv->actr_srv.msgq_in, &srv->actr_srv.actuator_cmd,
                        K_NO_WAIT) != 0) {
        k_msgq_purge(srv->actr_srv.msgq_in);
      }
    }

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
