/*
 * can_interface.h
 *
 * Created on: Jan 26, 2022 15:41
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <drivers/can.h>

#ifndef CAN_RX_MSGQ_SIZE
#define CAN_RX_MSGQ_SIZE 1024
#endif

#ifndef CAN_TX_TIMEOUT
#define CAN_TX_TIMEOUT 10
#endif

#ifndef CAN_SAMPLING_POINT
#define CAN_SAMPLING_POINT 875
#endif

typedef enum { DD_CAN0 = 0, DD_CAN1, DD_CAN_NUM } CanList;

// CAN
typedef struct {
  bool active;
  const struct device *device;
  uint32_t bitrate;
  struct k_msgq *msgq;
} CanDescriptor;

typedef struct {
  CanDescriptor descriptor[DD_CAN_NUM];
} CanDescription;

bool InitCan();
CanDescription *GetCanDescription();
CanDescriptor *GetCanDescriptor(CanList dev_id);
void PrintCanInitResult();

bool ConfigureCan(CanDescriptor *dd, enum can_mode mode, uint32_t bitrate,
                  struct zcan_filter zfilter);

int SendCanFrame(CanDescriptor *dd, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc);

#endif /* CAN_INTERFACE_H */
