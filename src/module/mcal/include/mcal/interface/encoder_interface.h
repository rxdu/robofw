/*
 * encoder_interface.h
 *
 * Created on: Feb 05, 2022 23:25
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef ENCODER_INTERFACE_H
#define ENCODER_INTERFACE_H

#include <drivers/encoder.h>

typedef enum {
  DD_ENCODER0 = 0,
  DD_ENCODER1,
  DD_ENCODER2,
  DD_ENCODER3,
  DD_ENCODER_NUM
} EncoderList;

// ENCODER
typedef struct {
  bool active;
  const struct device *device;
} EncoderDescriptor;

typedef struct {
  EncoderDescriptor descriptor[DD_ENCODER_NUM];
} EncoderDescription;

bool InitEncoder();
EncoderDescription *GetEncoderDescription();
EncoderDescriptor *GetEncoderDescriptor(EncoderList dev_id);
void PrintEncoderInitResult();

uint32_t GetEncoderCount(EncoderDescriptor *dd);
bool IsEncoderCountingUp(EncoderDescriptor *dd);

#endif /* ENCODER_INTERFACE_H */
