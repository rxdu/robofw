/*
 * led_cfg.c
 *
 * Created on: Mar 29, 2021 23:17
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

#define LED1_NODE DT_ALIAS(led1)
#define LED1 DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)

#define LED2_NODE DT_ALIAS(led2)
#define LED2 DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_FLAGS DT_GPIO_FLAGS(LED2_NODE, gpios)

typedef struct {
  const struct device *device;
  gpio_pin_t pin;
  gpio_flags_t flags;
} LEDChnDef;

typedef struct {
  LEDChnDef channel[2];
} LEDChnMapping;

static LEDChnMapping led_cfg;

bool SetupLed(void) {
  led_cfg.channel[0].device = device_get_binding(LED1);
  led_cfg.channel[0].pin = LED1_PIN;
  led_cfg.channel[0].flags = LED1_FLAGS;
  if (led_cfg.channel[0].device == NULL) return false;

  led_cfg.channel[1].device = device_get_binding(LED2);
  led_cfg.channel[1].pin = LED2_PIN;
  led_cfg.channel[1].flags = LED2_FLAGS;
  if (led_cfg.channel[1].device == NULL) return false;

  int ret = 0;
  for (int i = 0; i < 2; ++i) {
    ret = gpio_pin_configure(led_cfg.channel[i].device, led_cfg.channel[i].pin,
                             GPIO_OUTPUT_ACTIVE | led_cfg.channel[i].flags);
    if (ret < 0) return false;
  }

  return true;
}

void TurnOnLed(LedChannel led_chn) {
  uint8_t index = 0;
  switch (led_chn) {
    case USER_LED1:
    case USER_LED2:
      index = led_chn;
      break;
    default:
      return;
  }
  gpio_pin_set(led_cfg.channel[index].device, led_cfg.channel[index].pin, 1);
}

void TurnOffLed(LedChannel led_chn) {
  uint8_t index = 0;
  switch (led_chn) {
    case USER_LED1:
    case USER_LED2:
      index = led_chn;
      break;
    default:
      return;
  }
  gpio_pin_set(led_cfg.channel[index].device, led_cfg.channel[index].pin, 0);
}

void ToggleLed(LedChannel led_chn) {
  uint8_t index = 0;
  switch (led_chn) {
    case USER_LED1:
    case USER_LED2:
      index = led_chn;
      break;
    default:
      return;
  }
  gpio_pin_toggle(led_cfg.channel[index].device, led_cfg.channel[index].pin);
}
