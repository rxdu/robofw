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

#include "hwconfig/hwconfig.h"

#define SLEEP_TIME_MS 500

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (InitLed()) {
    printk("[INFO]: Initialized LED\n");
    PrintLedInitResult();
  } else {
    printk("[ERROR]: Failed to setup LED\n");
  }

  if (InitDio()) {
    printk("[INFO]: Initialized DIO\n");
    PrintDioInitResult();
  } else {
    printk("[ERROR]: Failed to setup DIO\n");
  }

  if (InitPwm()) {
    printk("[INFO]: Initialized PWM\n");
    PrintPwmInitResult();
  } else {
    printk("[ERROR]: Failed to setup PWM\n");
  }

  //   if (!SetupLed()) printk("[ERROR]: Failed to setup LED\n");
  //   //   if (!CanSetup(UPLINK_CAN, CAN_NORMAL_MODE, 1000000))
  //   //     printk("[ERROR]: Failed to setup CAN\n");

  //   if (!SetupPwm()) printk("[ERROR]: Failed to setup PWM\n");

  //   if (!SetupGpio()) printk("[ERROR]: Failed to setup GPIO\n");

  //   TurnOnLed(USER_LED1);
  //   TurnOnLed(USER_LED2);

  //   k_msleep(SLEEP_TIME_MS);

  //   SetGpio(DIR1, 1);
  //   SetGpio(EN1, 1);
  //   SetGpio(DIR2, 0);
  //   SetGpio(EN2, 1);

  //   SetPwmDutyCycle(PWM1, 0.7);
  //   SetPwmDutyCycle(PWM2, 0.7);

  LedDescription* led_desc = GetLedDescription();
  TurnOnLed(&led_desc->descriptor[0]);
  TurnOffLed(&led_desc->descriptor[1]);

  PwmDescription* pwm_desc = GetPwmDescription();
  SetPwmDutyCycle(&pwm_desc->descriptor[0], 0.55);
  SetPwmDutyCycle(&pwm_desc->descriptor[1], 0.68);

  uint8_t count = 0;

  while (1) {
    // if (count % 2 == 0)
    //   ToggleLed(USER_LED1);
    // else if (count % 2 == 1)
    //   ToggleLed(USER_LED2);

    // ToggleGpio(EN1);
    ToggleLed(&led_desc->descriptor[0]);
    ToggleLed(&led_desc->descriptor[1]);

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
