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

#include "drivers/encoder.h"

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (!InitRobot()) {
    printk("[ERROR] Failed to initialize robot\n");
    ShowRobotPanic();
  }
}
