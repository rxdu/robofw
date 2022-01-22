/*
 * init_gpio.c
 *
 * Created on: Jan 22, 2022 22:24
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/init_dio.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>

static DioDescription dio_desc;

bool InitDio() {
#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio0), okay)
  dio_desc.descriptor[0].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio0), gpios));
  dio_desc.descriptor[0].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio0), gpios);
  dio_desc.descriptor[0].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio0), gpios);
  if (!dio_desc.descriptor[0].dev) {
    printk("DIO0: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[0].active = true;
#else
  dio_desc.descriptor[0].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio1), okay)
  dio_desc.descriptor[1].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio1), gpios));
  dio_desc.descriptor[1].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio1), gpios);
  dio_desc.descriptor[1].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio1), gpios);
  if (!dio_desc.descriptor[1].dev) {
    printk("DIO1: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[1].active = true;
#else
  dio_desc.descriptor[1].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio2), okay)
  dio_desc.descriptor[2].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio2), gpios));
  dio_desc.descriptor[2].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio2), gpios);
  dio_desc.descriptor[2].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio2), gpios);
  if (!dio_desc.descriptor[2].dev) {
    printk("DIO1: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[2].active = true;
#else
  dio_desc.descriptor[2].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio3), okay)
  dio_desc.descriptor[3].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio3), gpios));
  dio_desc.descriptor[3].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio3), gpios);
  dio_desc.descriptor[3].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio3), gpios);
  if (!dio_desc.descriptor[3].dev) {
    printk("DIO1: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[3].active = true;
#else
  dio_desc.descriptor[3].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio4), okay)
  dio_desc.descriptor[4].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio4), gpios));
  dio_desc.descriptor[4].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio4), gpios);
  dio_desc.descriptor[4].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio4), gpios);
  if (!dio_desc.descriptor[4].dev) {
    printk("DIO1: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[4].active = true;
#else
  dio_desc.descriptor[4].active = false;
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_dio5), okay)
  dio_desc.descriptor[5].dev =
      device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(dd_dio5), gpios));
  dio_desc.descriptor[5].pin = DT_GPIO_PIN(DT_NODELABEL(dd_dio5), gpios);
  dio_desc.descriptor[5].flags = DT_GPIO_FLAGS(DT_NODELABEL(dd_dio5), gpios);
  if (!dio_desc.descriptor[5].dev) {
    printk("DIO1: Device driver not found.\n");
    return false;
  }
  dio_desc.descriptor[5].active = true;
#else
  dio_desc.descriptor[5].active = false;
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