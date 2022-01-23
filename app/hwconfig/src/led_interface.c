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
  if (!dd->active) return;
  gpio_pin_set(dd->device, dd->pin, 1);
  //   led_on(dd->device, dd->pin);
}

void TurnOffLed(LedDescriptor* dd) {
  if (!dd->active) return;
  gpio_pin_set(dd->device, dd->pin, 0);
  //   led_off(dd->device, dd->pin);
}

void ToggleLed(LedDescriptor* dd) {
  if (!dd->active) return;
  gpio_pin_toggle(dd->device, dd->pin);
  //   led_blink(dd->device, dd->pin, 500, 500);
}