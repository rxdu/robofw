/*
 * coordinator_service.h
 *
 * Created on: Jan 29, 2022 15:37
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef SYSTEM_SERVICE_H
#define SYSTEM_SERVICE_H

#include "interface/service.h"

#include "receiver/receiver_service.h"
#include "mcal/interface/led_interface.h"

typedef struct {
  Vector3f linear;
  Vector3f angular;
} __attribute__((aligned(8))) DesiredMotion;

typedef struct {
  LedDescriptor *dd_led_status;
} CoordinatorSrvConf;

typedef struct {
  struct k_msgq *desired_motion_msgq;
} CoordinatorSrvData;

struct ReceiverInterface;
struct ActuatorInterface;

typedef struct {
  struct k_msgq *desired_motion_msgq_out;
} CoordinatorInterface;

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  CoordinatorSrvConf sconf;
  CoordinatorSrvData sdata;

  // dependent interfaces
  struct {
    struct ReceiverInterface *receiver_interface;
    struct ActuatorInterface *actuator_interface;
  } dependencies;

  // interface
  CoordinatorInterface interface;
} CoordinatorServiceDef;

bool StartCoordinatorService(CoordinatorServiceDef *cfg);

#endif /* SYSTEM_SERVICE_H */
