/*
 * encoder_interface.c
 *
 * Created on: Feb 05, 2022 23:29
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "mcal/interface/encoder_interface.h"

#include <assert.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>
#include <sys/printk.h>

#define initialize_encoder(x, node_label, desc)             \
  {                                                         \
    encoder_desc.descriptor[x].device =                     \
        device_get_binding(DT_LABEL(DT_ALIAS(node_label))); \
    if (!desc.descriptor[x].device) {                       \
      printk("[ERROR] ENCODER device driver not found\n");  \
      return false;                                         \
    }                                                       \
    desc.descriptor[x].active = true;                       \
  }

static EncoderDescription encoder_desc;

bool InitEncoder() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_ENCODER_NUM; ++i)
    encoder_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xencoder0), okay)
  initialize_encoder(0, xencoder0, encoder_desc);
#endif

  //   encoder_desc.descriptor[0].device =
  //       device_get_binding(DT_LABEL(DT_ALIAS(xencoder0)));

#if DT_NODE_HAS_STATUS(DT_ALIAS(xencoder1), okay)
  initialize_encoder(1, xencoder1, encoder_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xencoder2), okay)
  initialize_encoder(2, xencoder2, encoder_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xencoder3), okay)
  initialize_encoder(3, xencoder3, encoder_desc);
#endif

  printk("[INFO] Initialized Encoder\n");
  PrintEncoderInitResult();

  return true;
}

EncoderDescription* GetEncoderDescription() { return &encoder_desc; }

EncoderDescriptor* GetEncoderDescriptor(EncoderList dev_id) {
  assert(dev_id < DD_ENCODER_NUM);
  return &encoder_desc.descriptor[dev_id];
}

void PrintEncoderInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_ENCODER_NUM; ++i) {
    if (encoder_desc.descriptor[i].active) {
      count++;
      printk(" - [xENCODER%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}

uint32_t GetEncoderCount(EncoderDescriptor* dd) {
  if (!dd->active) {
    printk("[xENCODER] Device inactive\n");
    return 0;
  }

  uint32_t count = 0;
  encoder_get_counts(dd->device, &count);
  return count;
}

bool IsEncoderCountingUp(EncoderDescriptor* dd) {
  if (!dd->active) {
    printk("[xENCODER] Device inactive\n");
    return false;
  }

  bool is_up = false;
  encoder_is_counting_up(dd->device, &is_up);
  return is_up;
}