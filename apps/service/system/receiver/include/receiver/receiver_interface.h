/*
 * receiver_interface.h
 *
 * Created on: Jan 29, 2022 16:58
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef RECEIVER_INTERFACE_H
#define RECEIVER_INTERFACE_H

#define RECEIVER_CHANNEL_NUMBER 16

typedef struct {
  bool signal_lost;
  float channels[RECEIVER_CHANNEL_NUMBER];  // scaled to [-1, 1]
} ReceiverData;

#endif /* RECEIVER_INTERFACE_H */
