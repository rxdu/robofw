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
#include "mcal/interface/encoder_interface.h"

#define ENCODER_CHANNEL_NUMBER 2
typedef struct {
  int32_t rpms[ENCODER_CHANNEL_NUMBER];
} __attribute__((aligned(8))) EstimatedSpeed;

typedef struct {
  uint8_t active_encoder_num;
  EncoderDescriptor *dd_encoders[ENCODER_CHANNEL_NUMBER];
  uint32_t pulse_per_round[ENCODER_CHANNEL_NUMBER];
} EncoderSrvConf;

typedef struct {
  struct k_msgq *encoder_rpm_msgq;
} EncoderSrvData;

struct EncoderInterface {
  struct k_msgq *rpm_msgq_out;
};

typedef struct {
  // thread config
  ThreadConfig main_tconf;
  ThreadConfig tim_tconf;

  // service config
  EncoderSrvConf sconf;
  EncoderSrvData sdata;

  // no dependency

  // message queue for input/output
  struct EncoderInterface interface;
} EncoderServiceDef;

bool StartEncoderService(EncoderServiceDef *cfg);

#endif /* ENCODER_SERVICE_H */
