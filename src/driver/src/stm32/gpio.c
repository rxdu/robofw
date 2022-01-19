/*
 * gpio.c
 *
 * Created on: Jan 19, 2022 23:09
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "driver/interface.h"

#include <string.h>
#include <zephyr.h>

#include <stdio.h>

#include <devicetree.h>
#include <device.h>
#include <drivers/gpio.h>

static const struct device *gpioe;

bool SetupGpio(void) {
  gpioe = device_get_binding(DT_LABEL(DT_NODELABEL(gpioe)));
  if (!gpioe) {
    return false;
  }

  gpio_pin_configure(gpioe, 9, GPIO_OUTPUT | GPIO_PULL_UP);
  gpio_pin_configure(gpioe, 11, GPIO_OUTPUT | GPIO_PULL_UP);
  gpio_pin_configure(gpioe, 13, GPIO_OUTPUT | GPIO_PULL_UP);
  gpio_pin_configure(gpioe, 14, GPIO_OUTPUT | GPIO_PULL_UP);

  gpio_pin_set(gpioe, 9, 0);
  gpio_pin_set(gpioe, 11, 0);
  gpio_pin_set(gpioe, 13, 0);
  gpio_pin_set(gpioe, 14, 0);

  return true;
}

void SetGpio(GpioChannel gpio_num, uint8_t value) {
  gpio_pin_t gpin;
  switch (gpio_num) {
    case EN1:
      gpin = 9;
      break;
    case DIR1:
      gpin = 11;
      break;
    case EN2:
      gpin = 13;
      break;
    case DIR2:
      gpin = 14;
      break;
    default:
      return;
  }
  gpio_pin_set(gpioe, gpin, value);
}

void ToggleGpio(GpioChannel gpio_num) {
  gpio_pin_t gpin;
  switch (gpio_num) {
    case EN1:
      gpin = 9;
      break;
    case DIR1:
      gpin = 11;
      break;
    case EN2:
      gpin = 13;
      break;
    case DIR2:
      gpin = 14;
      break;
    default:
      return;
  }
  gpio_pin_toggle(gpioe, gpin);
}