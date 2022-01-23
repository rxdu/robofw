/*
 * uart_init.c
 *
 * Created on: Jan 23, 2022 22:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/uart_init.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>
#include <sys/printk.h>

#define initialize_uart(x, node_label, desc)                \
  {                                                         \
    desc.descriptor[x].device =                             \
        device_get_binding(DT_LABEL(DT_ALIAS(node_label))); \
    if (!desc.descriptor[x].device) {                       \
      printk("UART: Device driver not found.\n");           \
      return false;                                         \
    }                                                       \
    desc.descriptor[x].active = true;                       \
  }

static UartDescription uart_desc;

bool InitUart() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_UART_NUM; ++i) uart_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart0), okay)
  initialize_uart(0, dduart0, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart1), okay)
  initialize_uart(1, dduart1, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart2), okay)
  initialize_uart(2, dduart2, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart3), okay)
  initialize_uart(3, dduart3, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart4), okay)
  initialize_uart(4, dduart4, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart5), okay)
  initialize_uart(5, dduart5, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart6), okay)
  initialize_uart(6, dduart6, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(dduart7), okay)
  initialize_uart(7, dduart7, uart_desc);
#endif

  return true;
}

UartDescription* GetUartDescription() { return &uart_desc; }

void PrintUartInitResult() {
  for (int i = 0; i < DD_UART_NUM; ++i) {
    if (uart_desc.descriptor[i].active) {
      printk(" - [UART%d]: %s \n", i, "active");
    }
  }
}