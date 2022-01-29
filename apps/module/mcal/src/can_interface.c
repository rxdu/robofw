/*
 * can_interface.c
 *
 * Created on: Jan 26, 2022 17:09
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "mcal/interface/can_interface.h"

#include <assert.h>

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

CanDescriptor* GetCanDescriptor(CanList dev_id) {
  assert(dev_id < DD_CAN_NUM);
  return &can_desc.descriptor[dev_id];
}

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

bool ConfigureCan(CanDescriptor* dd, enum can_mode mode, uint32_t bitrate,
                  struct zcan_filter zfilter) {
  if (!dd->active) {
    printk("[xCAN] Device inactive\n");
    return false;
  }

  struct can_timing timing;
  int ret = 0;
  ret = can_calc_timing(dd->device, &timing, bitrate, CAN_SAMPLING_POINT);
  if (ret < 0) {
    printk("[xCAN] Failed to calc a valid timing!\n");
    return false;
  }
  ret = can_set_timing(dd->device, &timing, NULL);
  if (ret != 0) {
    printk("[xCAN] Failed to set timing!\n");
    return false;
  }

  if (can_configure(dd->device, mode, bitrate)) {
    printk("[xCAN] Failed to configure mode or bitrate!\n");
    return false;
  }

  ret = can_attach_msgq(dd->device, dd->msgq, &zfilter);
  if (ret == CAN_NO_FREE_FILTER) {
    printk("[xCAN] No filter available!\n");
    return false;
  }

  return true;
}

int SendCanFrame(CanDescriptor* dd, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc) {
  if (!dd->active) {
    printk("[xCAN] Device inactive\n");
    return false;
  }

  struct zcan_frame frame;
  if (is_std_id) {
    frame.id_type = CAN_STANDARD_IDENTIFIER;
  } else {
    frame.id_type = CAN_EXTENDED_IDENTIFIER;
  }
  frame.id = id;
  frame.rtr = CAN_DATAFRAME;
  frame.dlc = dlc;
  memcpy(frame.data, data, dlc);

  return can_send(dd->device, &frame, K_MSEC(CAN_TX_TIMEOUT), NULL, NULL);
}