/* 
 * messenger_service.h
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_MESSENGER_SERVICE_H
#define ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_MESSENGER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "interface/service.h"
#include "mcal/interface/can_interface.h"

typedef struct {
  CanDescriptor *dd_can;
} MessengerSrvConf;

typedef struct {
} MessengerSrvData;

struct MessengerInterface {
  struct k_msgq *desired_motion_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  MessengerSrvConf sconf;
  MessengerSrvData sdata;

  // interface
  struct MessengerInterface interface;
} MessengerServiceDef;

bool StartMessengerService(MessengerServiceDef *def);

#endif //ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_MESSENGER_SERVICE_H
