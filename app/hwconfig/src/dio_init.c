/*
 * dio_init.c
 *
 * Created on: Jan 22, 2022 22:24
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/dio_init.h"

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
      printk("DIO: Device driver not found.\n");                           \
      return false;                                                        \
    }                                                                      \
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

  return true;
}

DioDescription* GetDioDescription() { return &dio_desc; }

void PrintDioInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_DIO_NUM; ++i) {
    if (dio_desc.descriptor[i].active) {
      count++;
      printk(" - [xDIO%d]: %s \n", i, "active");
    }
  }
  printk(" - Number of active instances: %d\n", count);
}