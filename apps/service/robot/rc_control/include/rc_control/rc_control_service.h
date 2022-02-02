/*
 * rc_control_service.h
 *
 * Created on: Feb 01, 2022 14:46
 * Description:
 * 
 * Service:
 *  - Input: ReceiverData (msgq_in)
 *  - Output: MotionCommand (msgq_out)
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef RC_CONTROL_SERVICE_H
#define RC_CONTROL_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

typedef struct {
  // thread config
  int8_t priority;
  struct k_thread *thread;
  k_thread_stack_t *stack;
  size_t stack_size;
  k_timeout_t delay;
  uint32_t period_ms;

  // receiver config
  //   ActuatorType type;
  uint8_t active_motor_num;
  void *actuator_cfg;

  // message queue for input
  struct k_msgq *msgq_in;
  //   ActuatorCmd actuator_cmd;
} RcControlServiceConf;

bool StartRcControlService(RcControlServiceConf *cfg);

#endif /* RC_CONTROL_SERVICE_H */
