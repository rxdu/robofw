/*
 * led_interface.c
 *
 * Created on: Jan 23, 2022 12:59
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/led_interface.h"

void TurnOnLed(LedDescriptor* dd) {
  if (!dd->active) {
    printk("[xLED] Device inactive\n");
    return;
  }

  gpio_pin_set(dd->device, dd->pin, 1);
}

void TurnOffLed(LedDescriptor* dd) {
  if (!dd->active) {
    printk("[xLED] Device inactive\n");
    return;
  }

  gpio_pin_set(dd->device, dd->pin, 0);
}

void ToggleLed(LedDescriptor* dd) {
  if (!dd->active) {
    printk("[xLED] Device inactive\n");
    return;
  }

  gpio_pin_toggle(dd->device, dd->pin);
}