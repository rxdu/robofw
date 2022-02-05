/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

#define SLEEP_TIME_MS 50

#include "drivers/encoder.h"

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  const struct device *dev1 = device_get_binding("TIM1_ENCODER");
  if (dev1 == NULL) {
    printk("Failed to find encoder device 1\n");
  } else {
    printk("[INFO] Initialized encoder device 1\n");
  }

  const struct device *dev2 = device_get_binding("TIM3_ENCODER");
  if (dev2 == NULL) {
    printk("Failed to find encoder device 3\n");
  } else {
    printk("[INFO] Initialized encoder device 3\n");
  }

  uint32_t count1 = 0;
  bool is_up1 = false;
  uint32_t count2 = 0;
  bool is_up2 = false;
  while (1) {
    encoder_get_counts(dev1, &count1);
    encoder_is_counting_up(dev1, &is_up1);
    encoder_get_counts(dev2, &count2);
    encoder_is_counting_up(dev2, &is_up2);

    printk("1: count %s: %d, 2: count %s: %d\n", is_up1 ? "up" : "down", count1,
           is_up2 ? "up" : "down", count2);
    k_msleep(500);
  }
}
