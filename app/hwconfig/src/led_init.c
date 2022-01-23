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
#include <sys/printk.h>

#define initialize_led(x, node_label, desc)                                   \
  {                                                                           \
    desc.descriptor[x].device =                                               \
        device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(node_label), gpios));   \
    desc.descriptor[x].pin = DT_GPIO_PIN(DT_NODELABEL(node_label), gpios);    \
    flags = DT_GPIO_FLAGS(DT_NODELABEL(node_label), gpios);                   \
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

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led0), okay)
  initialize_led(0, dd_led0, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led1), okay)
  initialize_led(1, dd_led1, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led2), okay)
  initialize_led(2, dd_led2, led_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_led3), okay)
  initialize_led(3, dd_led3, led_desc);
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