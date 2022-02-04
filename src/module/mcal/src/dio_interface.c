/*
 * dio_interface.c
 *
 * Created on: Jan 23, 2022 12:56
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "mcal/interface/dio_interface.h"

#include <assert.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>

#define initialize_dio(x, node_label, desc)                                \
  {                                                                        \
    desc.descriptor[x].device =                                            \
        device_get_binding(DT_GPIO_LABEL(DT_ALIAS(node_label), gpios));    \
    desc.descriptor[x].pin = DT_GPIO_PIN(DT_ALIAS(node_label), gpios);     \
    desc.descriptor[x].flags = DT_GPIO_FLAGS(DT_ALIAS(node_label), gpios); \
    if (!desc.descriptor[x].device) {                                      \
      printk("[ERROR] DIO device driver not found\n");                     \
      return false;                                                        \
    }                                                                      \
    gpio_pin_configure(desc.descriptor[x].device, desc.descriptor[x].pin,  \
                       desc.descriptor[x].flags);                          \
    desc.descriptor[x].active = true;                                      \
  }

static DioDescription dio_desc;

bool InitDio() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_DIO_NUM; ++i) dio_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio0), okay)
  initialize_dio(0, xdio0, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio1), okay)
  initialize_dio(1, xdio1, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio2), okay)
  initialize_dio(2, xdio2, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio3), okay)
  initialize_dio(3, xdio3, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio4), okay)
  initialize_dio(4, xdio4, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xdio5), okay)
  initialize_dio(5, xdio5, dio_desc);
#endif

  printk("[INFO] Initialized DIO\n");
  PrintDioInitResult();

  return true;
}

DioDescription *GetDioDescription() { return &dio_desc; }

DioDescriptor *GetDioDescriptor(DioList dev_id) {
  assert(dev_id < DD_DIO_NUM);
  return &dio_desc.descriptor[dev_id];
}

void PrintDioInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_DIO_NUM; ++i) {
    if (dio_desc.descriptor[i].active) {
      count++;
      printk(" - [xDIO%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}

void ConfigureDio(DioDescriptor *dd, gpio_flags_t flags) {
  if (!dd->active) {
    printk("[xDIO] Device inactive\n");
    return;
  }
  gpio_pin_configure(dd->device, dd->pin, flags);
}

void SetDio(DioDescriptor *dd, uint8_t value) {
  if (!dd->active) {
    printk("[xDIO] Device inactive\n");
    return;
  }
  gpio_pin_set(dd->device, dd->pin, value);
}

void ToggleDio(DioDescriptor *dd) {
  if (!dd->active) {
    printk("[xDIO] Device inactive\n");
    return;
  }
  gpio_pin_toggle(dd->device, dd->pin);
}