/*
 * messenger_service.h
 *
 * Created on 3/16/22 10:22 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef MESSENGER_MESSENGER_SERVICE_H
#define MESSENGER_MESSENGER_SERVICE_H

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
  struct k_msgq *robot_state_msgq;
  struct k_msgq *supervisor_cmd_msgq;
  struct k_msgq *desired_motion_msgq;
} MessengerSrvData;

struct ActuatorInterface;
struct SpeedControlInterface;

struct MessengerInterface {
  struct k_msgq *robot_state_msgq_in;
  struct k_msgq *supervisor_cmd_msgq_out;
  struct k_msgq *desired_motion_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig rx_tconf;
  ThreadConfig tx_tconf;

  // service config
  MessengerSrvConf sconf;
  MessengerSrvData sdata;

  // dependent interfaces
  struct {
    struct ActuatorInterface *actuator_interface;
    struct SpeedControlInterface *speed_control_interface;
  } dependencies;

  // interface
  struct MessengerInterface interface;
} MessengerServiceDef;

bool StartMessengerService(MessengerServiceDef *def);

#endif /* MESSENGER_MESSENGER_SERVICE_H */
