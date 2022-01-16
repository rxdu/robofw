/*
 * can_cfg.c
 *
 * Created on: Feb 11, 2021 09:51
 * Description:
 *
 * Copyright (c) 2021 Weston Robot
 */

#include <driver/interface.h>

#include <stdio.h>
#include <strings.h>

#include <device.h>

CAN_DEFINE_MSGQ(can_msgq1, 2048);
CAN_DEFINE_MSGQ(can_msgq2, 2048);

typedef struct {
  const struct device *device;
  const struct zcan_filter rx_filter;
  struct k_msgq *msgq;
} CanMapping;

typedef struct {
  CanMapping channels[2];
} CanConfig;

static CanConfig can_cfg = {
    // CAN1
    .channels[0] =
        {
            .rx_filter = {.id_type = CAN_EXTENDED_IDENTIFIER,
                          .rtr = CAN_DATAFRAME,
                          .rtr_mask = 1,
                          .id_mask = CAN_EXT_ID_MASK},
        },
    // CAN2
    .channels[1] = {
        .rx_filter = {.id_type = CAN_EXTENDED_IDENTIFIER,
                      .rtr = CAN_DATAFRAME,
                      .rtr_mask = 1,
                      .id_mask = CAN_EXT_ID_MASK},
    }};

bool SetupCan(CanChannel chn, CANMode mode, uint32_t baudrate) {
  if (chn == DOWNLINK_CAN) {
    can_cfg.channels[chn].device =
        // device_get_binding(DT_LABEL(DT_NODELABEL(can1)));
        device_get_binding(DT_CHOSEN_ZEPHYR_CAN_PRIMARY_LABEL);
    if (!can_cfg.channels[chn].device) {
      printf("CAN1: Device driver not found.\n");
      return false;
    }
    can_cfg.channels[chn].msgq = &can_msgq1;
  } else if (chn == UPLINK_CAN) {
    can_cfg.channels[chn].device =
        device_get_binding(DT_LABEL(DT_NODELABEL(can2)));
    if (!can_cfg.channels[chn].device) {
      printf("CAN2: Device driver not found.\n");
      return false;
    }
    can_cfg.channels[chn].msgq = &can_msgq2;
  }

  if (can_configure(can_cfg.channels[chn].device, mode, baudrate)) return false;
  int ret = 0;
  ret =
      can_attach_msgq(can_cfg.channels[chn].device, can_cfg.channels[chn].msgq,
                      &can_cfg.channels[chn].rx_filter);
  if (ret == CAN_NO_FREE_FILTER) {
    printf("Error, no filter available!\n");
    return false;
  }
  return true;
}

CANMsgQueue *GetCanMessageQueue(CanChannel chn) {
  return can_cfg.channels[chn].msgq;
}

// static void CanTxIrqCallback(uint32_t error_flags, void *arg) {
//   char *sender = (char *)arg;

//   if (error_flags) {
//     printk("Callback! error-code: %d\nSender: %s\n", error_flags, sender);
//   }
// }

int SendCanFrame(CanChannel chn, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc) {
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
  return can_send(can_cfg.channels[chn].device, &frame, K_MSEC(1), NULL, NULL);
  //   return can_send(can_cfg.channels[chn].device, &frame, K_FOREVER,
  //                   CanTxIrqCallback, NULL);
}