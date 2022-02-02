/*
 * light_service.c
 *
 * Created on: Feb 01, 2022 15:43
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "light/light_service.h"

K_MSGQ_DEFINE(light_cmd_queue, sizeof(LightCmd), 1, 8);

static void LightServiceLoop(void *p1, void *p2, void *p3);

bool StartLightService(LightServiceConf *cfg) {
  // init hardware
  ConfigureDio(cfg->light_cfg->dd_dio_front, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(cfg->light_cfg->dd_dio_front, 0);

  cfg->msgq_in = &light_cmd_queue;

  // create and start thread
  k_thread_create(cfg->thread, cfg->stack, cfg->stack_size, LightServiceLoop,
                  cfg, NULL, NULL, cfg->priority, 0, cfg->delay);

  return true;
}

void LightServiceLoop(void *p1, void *p2, void *p3) {
  LightServiceConf *cfg = (LightServiceConf *)p1;
  while (1) {
    if (k_msgq_get(cfg->msgq_in, &cfg->light_cmd, K_FOREVER) == 0) {
      bool cmd_front = cfg->light_cmd.lights[0];
      if (cmd_front) {
        SetDio(cfg->light_cfg->dd_dio_front, 1);
        printk("Set light on...\n");
      } else {
        SetDio(cfg->light_cfg->dd_dio_front, 0);
        printk("Set light off...\n");
      }
    }
    if (cfg->period_ms > 0) k_msleep(cfg->period_ms);
  }
}
