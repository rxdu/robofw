/*
 * led_init.c
 *
 * Created on: Jan 22, 2022 23:05
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/led_init.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/led.h>
#include <sys/printk.h>

#define initialize_led(x, node_label, desc)                                   \
  {                                                                           \
    desc.descriptor[x].device =                                               \
        device_get_binding(DT_GPIO_LABEL(DT_ALIAS(node_label), gpios));       \
    desc.descriptor[x].pin = DT_GPIO_PIN(DT_ALIAS(node_label), gpios);        \
    flags = DT_GPIO_FLAGS(DT_ALIAS(node_label), gpios);                       \
    if (!desc.descriptor[x].device) {                                         \
      printk("LED: Device driver not found.\n");                              \
      return false;                                                           \
    }                                                                         \
    ret =                                                                     \
        gpio_pin_configure(desc.descriptor[x].device, desc.descriptor[x].pin, \
                           flags | GPIO_OUTPUT_ACTIVE);                       \
    if (ret != 0) {                                                           \
      printk("LED: Failed to set pin to GPIO_OUTPUT_ACTIVE mode.\n");         \
      return false;                                                           \
    }                                                                         \
    desc.descriptor[x].active = true;                                         \
  }

static LedDescription led_desc;

bool InitLed() {
  int ret = 0;
  gpio_flags_t flags;

  // avoid compiler warning
  (void)ret;
  (void)flags;

  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_LED_NUM; ++i) led_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xled0), okay)
  initialize_led(0, xled0, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xled1), okay)
  initialize_led(1, xled1, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xled2), okay)
  initialize_led(2, xled2, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xled3), okay)
  initialize_led(3, xled3, led_desc);
#endif

  return true;
}

LedDescription* GetLedDescription() { return &led_desc; }

void PrintLedInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_LED_NUM; ++i) {
    if (led_desc.descriptor[i].active) {
      count++;
      printk(" - [xLED%d]: %s \n", i, "active");
    }
  }
  printk(" - Number of active instances: %d\n", count);
}