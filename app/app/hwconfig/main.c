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

  LedDescription* led_desc = GetLedDescription();
  TurnOnLed(&led_desc->descriptor[0]);
  TurnOffLed(&led_desc->descriptor[1]);

  DioDescription* dio_desc = GetDioDescription();
  ConfigureDio(&dio_desc->descriptor[0], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[1], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[2], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[3], GPIO_OUTPUT | GPIO_PULL_UP);
  SetDio(&dio_desc->descriptor[0], 1);
  SetDio(&dio_desc->descriptor[1], 1);
  SetDio(&dio_desc->descriptor[2], 1);
  SetDio(&dio_desc->descriptor[3], 0);

  PwmDescription* pwm_desc = GetPwmDescription();
  SetPwmDutyCycle(&pwm_desc->descriptor[0], 0.5);
  SetPwmDutyCycle(&pwm_desc->descriptor[1], 0.5);

  uint8_t count = 0;

  while (1) {
    // ToggleGpio(EN1);
    ToggleLed(&led_desc->descriptor[0]);
    ToggleLed(&led_desc->descriptor[1]);

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
