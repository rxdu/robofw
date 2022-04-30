/*
 * type.h
 *
 * Created on 4/30/22 11:24 AM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_INTERFACE_INCLUDE_INTERFACE_TYPE_H
#define ROBOFW_SRC_SERVICE_INTERFACE_INCLUDE_INTERFACE_TYPE_H

#include <stdbool.h>
#include <stdint.h>

#include <device.h>
#include <zephyr.h>

typedef struct {
  k_tid_t tid;
  struct k_thread thread;
  int8_t priority;
  uint32_t delay_ms;
  uint32_t period_ms;
} ThreadConfig;

typedef struct {
  float x;
  float y;
  float z;
} Vector3f;

typedef enum {
  kNonSupervised = 0,
  kSupervisedPwm = 1,
  kSupervisedRpm = 2
} SupervisedMode;

typedef struct {
  SupervisedMode supervised_mode;
} __attribute__((aligned(8))) SupervisorCommand;

enum RobotControlMode { kControlModeRC = 0, kControlModeCAN };

typedef struct {
  bool rc_connected;
  bool estop_triggered;
  enum RobotControlMode control_mode;
  uint8_t sup_mode;
} __attribute__((aligned(8))) RobotState;

#define FLOAT_VALUE_DELTA 0.00001f

#endif  // ROBOFW_SRC_SERVICE_INTERFACE_INCLUDE_INTERFACE_TYPE_H
