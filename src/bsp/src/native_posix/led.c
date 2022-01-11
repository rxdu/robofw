/*
 * led.c
 *
 * Created on: Feb 17, 2021 13:57
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "driver/interface.h"

#include <string.h>
#include <zephyr.h>

#include <stdio.h>

#include <devicetree.h>
#include <device.h>
#include <drivers/gpio.h>

typedef struct {
  const struct device *dev_led;
} LEDChnDef;

typedef struct {
  LEDChnDef channel[5];
} LEDChnMapping;

LEDChnMapping led;

bool LedSetup(void) {
  return true;
}

void ToggleLed(LedIndex led_num) {

}
