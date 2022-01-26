/*
 * can_interface.h
 *
 * Created on: Jan 26, 2022 15:41
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include "hwconfig/can_init.h"

bool ConfigureCan(CanDescriptor* dd, enum can_mode mode, uint32_t bitrate,
                  struct zcan_filter zfilter);

int SendCanFrame(CanDescriptor* dd, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc);

#endif /* CAN_INTERFACE_H */
