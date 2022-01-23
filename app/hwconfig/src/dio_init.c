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

#define initialize_dio(x, node_label, desc)                                    \
  {                                                                            \
    desc.descriptor[x].device =                                                   \
        device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(node_label), gpios));    \
    desc.descriptor[x].pin = DT_GPIO_PIN(DT_NODELABEL(node_label), gpios);     \
    desc.descriptor[x].flags = DT_GPIO_FLAGS(DT_NODELABEL(node_label), gpios); \
    if (!desc.descriptor[x].device) {                                             \
      printk("DIO: Device driver not found.\n");                               \
      return false;                                                            \
    }                                                                          \
    desc.descriptor[x].active = true;                                          \
  }

static DioDescription dio_desc;

bool InitDio() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_DIO_NUM; ++i) dio_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio0), okay)
  initialize_dio(0, dd_dio0, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio1), okay)
  initialize_dio(1, dd_dio1, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio2), okay)
  initialize_dio(2, dd_dio2, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio3), okay)
  initialize_dio(3, dd_dio3, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio4), okay)
  initialize_dio(4, dd_dio4, dio_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio5), okay)
  initialize_dio(5, dd_dio5, dio_desc);
#endif

  return true;
}

DioDescription* GetDioDescription() { return &dio_desc; }

void PrintDioInitResult() {
  for (int i = 0; i < DD_DIO_NUM; ++i) {
    printk(" - [DIO%d]: %s \n", i,
           dio_desc.descriptor[i].active ? "active" : "inactive");
  }
}