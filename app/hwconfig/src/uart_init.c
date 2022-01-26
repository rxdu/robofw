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
      printk("[ERROR] UART device driver not found\n");     \
      return false;                                         \
    }                                                       \
    desc.descriptor[x].active = true;                       \
  }

static UartDescription uart_desc;

bool InitUart() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_UART_NUM; ++i) uart_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart0), okay)
  initialize_uart(0, xuart0, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart1), okay)
  initialize_uart(1, xuart1, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart2), okay)
  initialize_uart(2, xuart2, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart3), okay)
  initialize_uart(3, xuart3, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart4), okay)
  initialize_uart(4, xuart4, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart5), okay)
  initialize_uart(5, xuart5, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart6), okay)
  initialize_uart(6, xuart6, uart_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xuart7), okay)
  initialize_uart(7, xuart7, uart_desc);
#endif

  printk("[INFO] Initialized UART\n");
  PrintUartInitResult();

  return true;
}

UartDescription* GetUartDescription() { return &uart_desc; }

void PrintUartInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_UART_NUM; ++i) {
    if (uart_desc.descriptor[i].active) {
      count++;
      printk(" - [xUART%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}