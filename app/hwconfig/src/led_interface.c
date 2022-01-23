/*
 * led_interface.c
 *
 * Created on: Jan 23, 2022 12:59
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/led_interface.h"

void TurnOnLed(LedDescriptor* dd) { gpio_pin_set(dd->device, dd->pin, 1); }

void TurnOffLed(LedDescriptor* dd) { gpio_pin_set(dd->device, dd->pin, 0); }

void ToggleLed(LedDescriptor* dd) { gpio_pin_toggle(dd->device, dd->pin); }