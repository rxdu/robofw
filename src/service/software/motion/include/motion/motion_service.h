/*
 * motion_service.h
 *
 * Created on 4/21/22 9:17 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_SERVICE_H
#define ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_SERVICE_H

#include "interface/service.h"

typedef struct {
  //  LedDescriptor *dd_led_status;
} MotionSrvConf;

typedef struct {
  struct k_msgq *desired_motion_msgq;
} MotionSrvData;

struct ReceiverInterface;
struct ActuatorInterface;

typedef struct {
  struct k_msgq *desired_motion_msgq_out;
} MotionInterface;

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  MotionSrvConf sconf;
  MotionSrvData sdata;

  // dependent interfaces
  struct {
    struct ReceiverInterface *receiver_interface;
    struct ActuatorInterface *actuator_interface;
  } dependencies;

  // interface
  MotionInterface interface;
} MotionServiceDef;

bool StartMotionService(MotionServiceDef *cfg);

#endif  // ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_SERVICE_H
