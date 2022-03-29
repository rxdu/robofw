/*
 * encoder_service.h
 *
 * Created on: Feb 06, 2022 16:33
 * Description:
 *
 * Assumption: using 16-bit timers
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef ENCODER_SERVICE_H
#define ENCODER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>

#include "interface/service.h"
#include "actuator/actuator_service.h"
#include "mcal/interface/encoder_interface.h"

typedef struct {
  int32_t rpms[ACTUATOR_CHANNEL_NUMBER];
} __attribute__((aligned(8))) EstimatedSpeed;

typedef struct {
  uint8_t active_encoder_num;
  EncoderDescriptor *dd_encoders[ACTUATOR_CHANNEL_NUMBER];
  uint32_t pulse_per_round[ACTUATOR_CHANNEL_NUMBER];
} EncoderSrvConf;

typedef struct {
  struct k_msgq *encoder_rpm_msgq;
} EncoderSrvData;

typedef struct {
  struct k_msgq *estimated_rpm_msgq_out;
} EncoderInterface;

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  EncoderSrvConf sconf;
  EncoderSrvData sdata;

  // no dependency

  // message queue for input/output
  EncoderInterface interface;
} EncoderServiceDef;

bool StartEncoderService(EncoderServiceDef *cfg);

#endif /* ENCODER_SERVICE_H */
