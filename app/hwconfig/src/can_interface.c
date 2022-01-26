/*
 * can_interface.c
 *
 * Created on: Jan 26, 2022 17:09
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "hwconfig/can_interface.h"

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