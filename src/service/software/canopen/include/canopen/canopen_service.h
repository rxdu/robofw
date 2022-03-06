/* 
 * canopen_service.h
 * 
 * Created on: Feb 09, 2022 15:49
 * Description: 
 * 
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef CANOPEN_SERVICE_H
#define CANOPEN_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/can_interface.h"

typedef struct {
	CanDescriptor *dd_can;
} CanDeviceConfig;

typedef struct {
	// thread config
	k_tid_t tid;
	int8_t priority;
	struct k_thread *thread;
	k_thread_stack_t *stack;
	size_t stack_size;
	k_timeout_t delay;
	uint32_t period_ms;

	// task-related config
	//   LedConf *led_cfg;
	//   ReceiverServiceDef *rcvr_srv;
	CanDeviceConfig *can_cfg;

	// message queue for input/output
	//   struct k_msgq *msgq_out;
	//   DesiredMotion desired_motion;
} CanopenServiceConf;

bool StartCanopenService(CanopenServiceConf *cfg);

#endif /* CANOPEN_SERVICE_H */
