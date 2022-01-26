/*
 * can_init.h
 *
 * Created on: Jan 26, 2022 15:21
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef CAN_INIT_H
#define CAN_INIT_H

#include <drivers/can.h>

#ifndef CAN_RX_MSGQ_SIZE
#define CAN_RX_MSGQ_SIZE 1024
#endif

#ifndef CAN_TX_TIMEOUT
#define CAN_TX_TIMEOUT 10
#endif

typedef enum { DD_CAN0 = 0, DD_CAN1, DD_CAN_NUM } CanList;

// CAN
typedef struct {
  bool active;
  const struct device *device;
  struct k_msgq *msgq;
} CanDescriptor;

typedef struct {
  CanDescriptor descriptor[DD_CAN_NUM];
} CanDescription;

bool InitCan();
CanDescription *GetCanDescription();
void PrintCanInitResult();

#endif /* CAN_INIT_H */
