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

#include "mcal/interface/led_interface.h"

enum RobotControlMode { kControlModeRC = 0, kControlModeCAN };

typedef struct {
  bool rc_connected;
  bool estop_triggered;
  enum RobotControlMode control_mode;
} __attribute__((aligned(8))) RobotState;

typedef struct {
  LedDescriptor *dd_led_status;
} CoordinatorSrvConf;

typedef struct {
  struct k_msgq *robot_state_msgq;
} CoordinatorSrvData;

struct ReceiverInterface;
struct MotionControlInterface;
struct MessengerInterface;

struct CoordinatorInterface {
  struct k_msgq *robot_state_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  CoordinatorSrvConf sconf;
  CoordinatorSrvData sdata;

  // dependent interfaces
  struct {
    struct ReceiverInterface *receiver_interface;
    struct MessengerInterface *messenger_interface;
    struct MotionControlInterface *motion_control_interface;
  } dependencies;

  // interface
  struct CoordinatorInterface interface;
} CoordinatorServiceDef;

bool StartCoordinatorService(CoordinatorServiceDef *cfg);

#endif /* SYSTEM_SERVICE_H */
