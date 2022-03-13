/* 
 * canopen_service.h
 * 
 * Created on: Feb 09, 2022 15:49
 * Description: 
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef CANOPEN_SERVICE_H
#define CANOPEN_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "interface/service.h"
#include "mcal/interface/can_interface.h"

typedef struct {
  CanDescriptor *dd_can;
} CanopenSrvConf;

typedef struct {
} CanopenSrvData;

struct CanopenInterface {
  struct k_msgq *desired_motion_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  CanopenSrvConf sconf;
  CanopenSrvData sdata;

  // interface
  struct CanopenInterface interface;
} CanopenServiceDef;

bool StartCanopenService(CanopenServiceDef *cfg);

#endif /* CANOPEN_SERVICE_H */
