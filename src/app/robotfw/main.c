/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/sensor.h>

#include "driver/interface.h"

#define SLEEP_TIME_MS 500

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (!SetupLed()) printk("[ERROR]: Failed to setup LED\n");
  //   if (!CanSetup(UPLINK_CAN, CAN_NORMAL_MODE, 1000000))
  //     printk("[ERROR]: Failed to setup CAN\n");

  //   if (!SetupPwm()) printk("[ERROR]: Failed to setup PWM\n");

  //   SetPwmDutyCycle(SERVO_PWM, 0.05);
  //   SetPwmDutyCycle(MOTOR_PWM, 0.05);

  TurnOnLed(USER_LED1);
  TurnOnLed(USER_LED2);

  k_msleep(SLEEP_TIME_MS);

  uint8_t count = 0;

  while (1) {
    if (count % 2 == 0)
      ToggleLed(USER_LED1);
    else if (count % 2 == 1)
      ToggleLed(USER_LED2);

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
