/*
 * dio_interface.c
 *
 * Created on: Jan 23, 2022 12:56
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/dio_interface.h"

void ConfigureDio(DioDescriptor* dd, gpio_flags_t flags) {
  if (!dd->active) {
    printk("[xDIO]: device inactive\n");
    return;
  }
  gpio_pin_configure(dd->device, dd->pin, dd->flags | flags);
}

void SetDio(DioDescriptor* dd, uint8_t value) {
  if (!dd->active) {
    printk("[xDIO]: device inactive\n");
    return;
  }
  gpio_pin_set(dd->device, dd->pin, value);
}

void ToggleDio(DioDescriptor* dd) {
  if (!dd->active) {
    printk("[xDIO]: device inactive\n");
    return;
  }
  gpio_pin_toggle(dd->device, dd->pin);
}