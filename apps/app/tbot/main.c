/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

#include "tbot/tbot_interface.h"
#include "receiver/receiver_service.h"

#define SLEEP_TIME_MS 500

#define TASK_PRIORITY_VIP -1  // Negative prio threads will not be pre-empted
#define TASK_PRIORITY_HIGHEST 1
#define TASK_PRIORITY_HIGH 2
#define TASK_PRIORITY_MID 3
#define TASK_PRIORITY_LOW 4

struct k_thread receiver_thread;
K_THREAD_STACK_DEFINE(receiver_service_stack, 1024);

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (!InitRobot()) {
    printk("[ERROR] Failed to initialize robot\n");
    while (true) {
      TurnOnLed(TBOT_LED_STATUS);
      TurnOnLed(TBOT_LED_USER1);
      TurnOnLed(TBOT_LED_USER2);
    }
  }

  SetMotorCmd(-0.5, -0.5);

  uint8_t count = 0;
  uint8_t data[] = "hello";
  //   uint8_t candata[] = {0x11, 0x22, 0x55, 0x66};
  struct zcan_frame rx_frame;

  (void)data;

  CanDescription *can_desc = GetCanDescription();

  typedef struct {
    ReceiverType type;
    int8_t priority;
    struct k_thread *thread;
    k_thread_stack_t *stack;
    size_t stack_size;
    k_timeout_t delay;

    void *rcvr_cfg;
    struct k_msgq *msgq;
  } ReceiverServiceConf;

  ReceiverServiceConf rcvr_srv;
  rcvr_srv.type = RCVR_SBUS;
  rcvr_srv.priority = TASK_PRIORITY_HIGH;
  rcvr_srv.thread = &receiver_thread;
  rcvr_srv.stack = receiver_service_stack;
  rcvr_srv.stack_size = K_THREAD_STACK_SIZEOF(receiver_service_stack);
  rcvr_srv.delay = K_NO_WAIT;

  SbusConf sbus_cfg;
  sbus_cfg.dev_id = TBOT_UART_SBUS;

  rcvr_srv.rcvr_cfg = &sbus_cfg;

  bool ret = StartReceiverService(&rcvr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start receiver service\n");
  }

  while (1) {
    ToggleLed(TBOT_LED_STATUS);
    // ToggleLed(DD_LED1);
    // ToggleLed(DD_LED2);

    // if (!StartUartAsyncSend(DD_UART0, data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n", uart_desc->descriptor[0].device->name);
    // }
    // if (!StartUartAsyncSend(DD_UART1, data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n",
    // uart_desc->descriptor[1].device->name);
    // }
    // if (k_sem_take(DD_UART0.rx_sem, K_MSEC(50)) == 0) {
    //   uint8_t ch;
    //   while (ring_buf_get(DD_UART0.ring_buffer, &ch, 1) !=
    //   0) {
    //     printk("%02x ", ch);
    //   }
    // }

    if (k_msgq_get(can_desc->descriptor[0].msgq, &rx_frame, K_MSEC(50)) == 0) {
      printk("CAN1 %02x: ", rx_frame.id);
      for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ", rx_frame.data[i]);
      printk("\n");
    }
    // printk("%s sending\n", can_desc->descriptor[0].device->name);
    // int ret = SendCanFrame(DD_CAN0, 0x121, true, candata, 4);
    // if (ret != CAN_TX_OK) {
    //   printk("%s send failed: %d\n", can_desc->descriptor[0].device->name,
    //   ret);
    // } else {
    //   //   printk("%s sent\n", can_desc->descriptor[0].device->name);
    // }

    // if (k_msgq_get(can_desc->descriptor[1].msgq, &rx_frame, K_MSEC(50)) == 0)
    // {
    //   printk("CAN2 %02x: ", rx_frame.id);
    //   for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ",
    //   rx_frame.data[i]); printk("\n");
    // }
    // if (SendCanFrame(DD_CAN1, 0x121, true, candata, 4) !=
    //     CAN_TX_OK) {
    // }

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
