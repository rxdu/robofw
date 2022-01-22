/*
 * init_led.c
 *
 * Created on: Jan 22, 2022 23:05
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/init_led.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>

static LedDescription led_desc;

bool InitLed() {
  int ret = 0;
  gpio_flags_t flags;

  // avoid compiler warning
  (void)ret;
  (void)flags;

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led0), okay)
  printk("LED0: setup led0.\n");
  led_desc.descriptor[0].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_led0), gpios));
  led_desc.descriptor[0].pin = DT_GPIO_PIN(DT_NODELABEL(dd_led0), gpios);
  flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_led0), gpios);
  if (!led_desc.descriptor[0].dev) {
    printk("LED0: Device driver not found.\n");
    return false;
  }
  ret =
      gpio_pin_configure(led_desc.descriptor[0].dev, led_desc.descriptor[0].pin,
                         flags | GPIO_OUTPUT_ACTIVE);
  if (ret != 0) {
    printk("LED0: Failed to set pin to GPIO_OUTPUT_ACTIVE mode.\n");
    return false;
  }
  led_desc.descriptor[0].active = true;
#else
  led_desc.descriptor[0].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led1), okay)
  led_desc.descriptor[1].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_led1), gpios));
  led_desc.descriptor[1].pin = DT_GPIO_PIN(DT_NODELABEL(dd_led1), gpios);
  flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_led1), gpios);
  if (!led_desc.descriptor[1].dev) {
    printk("LED1: Device driver not found.\n");
    return false;
  }
  ret =
      gpio_pin_configure(led_desc.descriptor[1].dev, led_desc.descriptor[1].pin,
                         flags | GPIO_OUTPUT_ACTIVE);
  if (ret != 0) {
    printk("LED1: Failed to set pin to GPIO_OUTPUT_ACTIVE mode.\n");
    return false;
  }
  led_desc.descriptor[1].active = true;
#else
  led_desc.descriptor[1].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led2), okay)
  led_desc.descriptor[2].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_led2), gpios));
  led_desc.descriptor[2].pin = DT_GPIO_PIN(DT_NODELABEL(dd_led2), gpios);
  flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_led2), gpios);
  if (!led_desc.descriptor[2].dev) {
    printk("LED1: Device driver not found.\n");
    return false;
  }
  ret =
      gpio_pin_configure(led_desc.descriptor[2].dev, led_desc.descriptor[2].pin,
                         flags | GPIO_OUTPUT_ACTIVE);
  if (ret != 0) {
    printk("LED2: Failed to set pin to GPIO_OUTPUT_ACTIVE mode.\n");
    return false;
  }
  led_desc.descriptor[2].active = true;
#else
  led_desc.descriptor[2].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led3), okay)
  led_desc.descriptor[3].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_led3), gpios));
  led_desc.descriptor[3].pin = DT_GPIO_PIN(DT_NODELABEL(dd_led3), gpios);
  flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_led3), gpios);
  if (!led_desc.descriptor[3].dev) {
    printk("LED3: Device driver not found.\n");
    return false;
  }
  ret =
      gpio_pin_configure(led_desc.descriptor[3].dev, led_desc.descriptor[3].pin,
                         flags | GPIO_OUTPUT_ACTIVE);
  if (ret != 0) {
    printk("LED3: Failed to set pin to GPIO_OUTPUT_ACTIVE mode.\n");
    return false;
  }
  led_desc.descriptor[3].active = true;
#else
  led_desc.descriptor[3].active = false;
#endif

  return true;
}

LedDescription* GetLedDescription() { return &led_desc; }

void PrintLedInitResult() {
  for (int i = 0; i < DD_LED_NUM; ++i) {
    printk(" - [LED%d]: %s \n", i,
           led_desc.descriptor[i].active ? "active" : "inactive");
  }
}