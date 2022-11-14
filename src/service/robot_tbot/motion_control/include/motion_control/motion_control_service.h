/*
 * motion_control_service.h
 *
 * Created on 4/21/22 9:17 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_CONTROL_SERVICE_H
#define ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_CONTROL_SERVICE_H

#include "interface/service.h"

typedef struct {
  float linear;
  float angular;
} __attribute__((aligned(8))) DesiredMotion;

typedef struct {
  //  DesiredRpm target_speed;
  //  EstimatedSpeed measured_speed;
} __attribute__((aligned(8))) MotionControlFeedback;

typedef struct {
  //  LedDescriptor *dd_led_status;
} MotionControlSrvConf;

typedef struct {
  struct k_msgq *desired_motion_msgq;
  struct k_msgq *control_feedback_msgq;
} MotionControlSrvData;

struct CoordinatorInterface;
struct SpeedControlInterface;

struct MotionControlInterface {
  struct k_msgq *desired_motion_msgq_in;
  struct k_msgq *control_feedback_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  MotionControlSrvConf sconf;
  MotionControlSrvData sdata;

  // dependent interfaces
  struct {
    struct CoordinatorInterface *coordinator_interface;
    struct SpeedControlInterface *speed_control_interface;
  } dependencies;

  // interface
  struct MotionControlInterface interface;
} MotionControlServiceDef;

bool StartMotionService(MotionControlServiceDef *def);

#endif  // ROBOFW_SRC_SERVICE_SOFTWARE_MOTION_INCLUDE_MOTION_MOTION_CONTROL_SERVICE_H
