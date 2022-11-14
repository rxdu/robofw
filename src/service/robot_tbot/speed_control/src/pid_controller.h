/*
 * pid_controller.h
 *
 * Created on 4/22/22 9:31 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_SOFTWARE_SPEED_CONTROL_SRC_PID_CONTROLLER_H
#define ROBOFW_SRC_SERVICE_SOFTWARE_SPEED_CONTROL_SRC_PID_CONTROLLER_H

typedef struct {
  // parameters
  float kp;
  float ki;
  float kd;
  float umax;
  float ts;

  // internal variables
  float integral;
  float error_last;
} PidControllerInstance;

void InitPidController(PidControllerInstance* inst);
float UpdatePidController(PidControllerInstance* inst, float ref, float meas);

#endif  // ROBOFW_SRC_SERVICE_SOFTWARE_SPEED_CONTROL_SRC_PID_CONTROLLER_H
