/*
 * led_cfg.c
 *
 * Created on: Mar 29, 2021 23:17
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "bsp/interface.h"

#include <string.h>
#include <zephyr.h>

#include <stdio.h>

#include <devicetree.h>
#include <device.h>
#include <drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)

#define LED1_NODE DT_ALIAS(led1)
#define LED1 DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)

#define LED2_NODE DT_ALIAS(led2)
#define LED2 DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_FLAGS DT_GPIO_FLAGS(LED2_NODE, gpios)

#define LED3_NODE DT_ALIAS(led3)
#define LED3 DT_GPIO_LABEL(LED3_NODE, gpios)
#define LED3_PIN DT_GPIO_PIN(LED3_NODE, gpios)
#define LED3_FLAGS DT_GPIO_FLAGS(LED3_NODE, gpios)

typedef struct {
  const struct device *device;
  gpio_pin_t pin;
} LedMapping;

typedef struct {
  LedMapping leds[4];
} LedConfig;

static LedConfig led_cfg;

bool SetupLed(void) {
  led_cfg.leds[0].device = device_get_binding(LED0);
  led_cfg.leds[0].pin = LED0_PIN;
  if (led_cfg.leds[0].device == NULL) return false;

  led_cfg.leds[1].device = device_get_binding(LED1);
  led_cfg.leds[1].pin = LED1_PIN;
  if (led_cfg.leds[1].device == NULL) return false;

  led_cfg.leds[2].device = device_get_binding(LED2);
  led_cfg.leds[2].pin = LED2_PIN;
  if (led_cfg.leds[2].device == NULL) return false;

  led_cfg.leds[3].device = device_get_binding(LED3);
  led_cfg.leds[3].pin = LED3_PIN;
  if (led_cfg.leds[3].device == NULL) return false;

  int ret = 0;
  ret = gpio_pin_configure(led_cfg.leds[0].device, LED0_PIN,
                           GPIO_OUTPUT_ACTIVE | LED0_FLAGS);
  if (ret < 0) return false;
  ret = gpio_pin_configure(led_cfg.leds[1].device, LED1_PIN,
                           GPIO_OUTPUT_ACTIVE | LED1_FLAGS);
  if (ret < 0) return false;
  ret = gpio_pin_configure(led_cfg.leds[2].device, LED2_PIN,
                           GPIO_OUTPUT_ACTIVE | LED2_FLAGS);
  if (ret < 0) return false;
  ret = gpio_pin_configure(led_cfg.leds[3].device, LED3_PIN,
                           GPIO_OUTPUT_ACTIVE | LED3_FLAGS);
  if (ret < 0) return false;
  return true;
}

void TurnOnLed(LedIndex led_chn) {
  switch (led_chn) {
    case USER_LED0:
      gpio_pin_set(led_cfg.leds[0].device, led_cfg.leds[0].pin, 1);
      break;
    case USER_LED1:
      gpio_pin_set(led_cfg.leds[1].device, led_cfg.leds[1].pin, 1);
      break;
    case USER_LED2:
      gpio_pin_set(led_cfg.leds[2].device, led_cfg.leds[2].pin, 1);
      break;
    case USER_LED3:
      gpio_pin_set(led_cfg.leds[3].device, led_cfg.leds[3].pin, 1);
      break;
    default:
      break;
  }
}

void TurnOffLed(LedIndex led_chn) {
  switch (led_chn) {
    case USER_LED0:
      gpio_pin_set(led_cfg.leds[0].device, led_cfg.leds[0].pin, 0);
      break;
    case USER_LED1:
      gpio_pin_set(led_cfg.leds[1].device, led_cfg.leds[1].pin, 0);
      break;
    case USER_LED2:
      gpio_pin_set(led_cfg.leds[2].device, led_cfg.leds[2].pin, 0);
      break;
    case USER_LED3:
      gpio_pin_set(led_cfg.leds[3].device, led_cfg.leds[3].pin, 0);
      break;
    default:
      break;
  }
}

void ToggleLed(LedIndex led_chn) {
  switch (led_chn) {
    case USER_LED0:
      gpio_pin_toggle(led_cfg.leds[0].device, led_cfg.leds[0].pin);
      break;
    case USER_LED1:
      gpio_pin_toggle(led_cfg.leds[1].device, led_cfg.leds[1].pin);
      break;
    case USER_LED2:
      gpio_pin_toggle(led_cfg.leds[2].device, led_cfg.leds[2].pin);
      break;
    case USER_LED3:
      gpio_pin_toggle(led_cfg.leds[3].device, led_cfg.leds[3].pin);
      break;
    default:
      break;
  }
}
