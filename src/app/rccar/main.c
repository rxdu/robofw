/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

#include "rccar_interface.h"
#include "mcal/hwconfig.h"

#define SLEEP_TIME_MS 50

#include "drivers/encoder.h"

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (!InitRobot()) {
    printk("[ERROR] Failed to initialize robot\n");
    ShowRobotPanic();
  }

  LedDescriptor *led0 = GetLedDescriptor(DD_LED0);
  LedDescriptor *led1 = GetLedDescriptor(DD_LED1);
  while (1) {
    ToggleLed(led0);
    ToggleLed(led1);

    k_msleep(1000);
  }
}
