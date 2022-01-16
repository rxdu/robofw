/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 500

/* The devicetree node identifier for the "led0" alias. */
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
#define LED1 DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led1 devicetree alias is not defined"
#define LED1 ""
#define PIN 0
#define FLAGS 0
#endif

#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2 DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_FLAGS DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led2 devicetree alias is not defined"
#define LED2 ""
#define PIN 0
#define FLAGS 0
#endif

// #if DT_NODE_HAS_STATUS(LED3_NODE, okay)
// #define LED3 DT_GPIO_LABEL(LED3_NODE, gpios)
// #define LED3_PIN DT_GPIO_PIN(LED3_NODE, gpios)
// #define LED3_FLAGS DT_GPIO_FLAGS(LED3_NODE, gpios)
// #else
// /* A build error here means your board isn't set up to blink an LED. */
// #error "Unsupported board: led3 devicetree alias is not defined"
// #define LED3 ""
// #define PIN 0
// #define FLAGS 0
// #endif

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  bool led_is_on = true;
  int ret;

  const struct device *dev_led1 = device_get_binding(LED1);
  const struct device *dev_led2 = device_get_binding(LED2);
  //   const struct device *dev_led3 = device_get_binding(LED3);

  //   if (dev_led1 == NULL || dev_led2 == NULL || dev_led3 == NULL) {
  if (dev_led1 == NULL || dev_led2 == NULL) {
    return;
  }

  ret = gpio_pin_configure(dev_led1, LED1_PIN, GPIO_OUTPUT_ACTIVE | LED1_FLAGS);
  ret = gpio_pin_configure(dev_led2, LED2_PIN, GPIO_OUTPUT_ACTIVE | LED2_FLAGS);
  //   ret = gpio_pin_configure(dev_led3, LED3_PIN, GPIO_OUTPUT_ACTIVE |
  //   LED3_FLAGS);

  if (ret < 0) {
    return;
  }

  gpio_pin_set(dev_led1, LED1_PIN, true);
  gpio_pin_set(dev_led2, LED2_PIN, false);

  while (1) {
    // gpio_pin_set(dev_led1, LED1_PIN, (int)led_is_on);
    // gpio_pin_set(dev_led2, LED2_PIN, (int)led_is_on);
    gpio_pin_toggle(dev_led1, LED1_PIN);
    gpio_pin_toggle(dev_led2, LED2_PIN);
    // gpio_pin_set(dev_led3, LED3_PIN, (int)led_is_on);

    led_is_on = !led_is_on;
    k_msleep(SLEEP_TIME_MS);
  }
}
