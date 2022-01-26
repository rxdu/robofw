/*
 * can_init.c
 *
 * Created on: Jan 26, 2022 15:27
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "hwconfig/can_init.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/can.h>
#include <sys/printk.h>

#define initialize_can(x, node_label, desc, msgqx)          \
  {                                                         \
    desc.descriptor[x].device =                             \
        device_get_binding(DT_LABEL(DT_ALIAS(node_label))); \
    if (!desc.descriptor[x].device) {                       \
      printk("[ERROR] CAN device driver not found\n");      \
      return false;                                         \
    }                                                       \
    desc.descriptor[x].msgq = &msgqx;                       \
    desc.descriptor[x].active = true;                       \
  }

static CanDescription can_desc;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xcan0), okay)
CAN_DEFINE_MSGQ(can_msgq1, CAN_RX_MSGQ_SIZE);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xcan1), okay)
CAN_DEFINE_MSGQ(can_msgq2, CAN_RX_MSGQ_SIZE);
#endif

bool InitCan() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_CAN_NUM; ++i) can_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xcan0), okay)
  initialize_can(0, xcan0, can_desc, can_msgq1);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xcan1), okay)
  initialize_can(1, xcan1, can_desc, can_msgq2);
#endif

  printk("[INFO] Initialized CAN\n");
  PrintCanInitResult();

  return true;
}

CanDescription* GetCanDescription() { return &can_desc; }

void PrintCanInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_CAN_NUM; ++i) {
    if (can_desc.descriptor[i].active) {
      count++;
      printk(" - [xCAN%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}