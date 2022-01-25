/*
 * uart_init.h
 *
 * Created on: Jan 23, 2022 22:53
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef UART_INIT_H
#define UART_INIT_H

#include <zephyr.h>
#include <drivers/uart.h>

typedef enum {
  DD_UART0 = 0,
  DD_UART1,
  DD_UART2,
  DD_UART3,
  DD_UART4,
  DD_UART5,
  DD_UART6,
  DD_UART7,
  DD_UART_NUM
} UartList;

// LED
typedef struct {
  bool active;
  const struct device* device;
  struct k_sem rx_sem;
  struct k_sem tx_sem;
  struct uart_config config;
} UartDescriptor;

typedef struct {
  UartDescriptor descriptor[DD_UART_NUM];
} UartDescription;

bool InitUart();
UartDescription* GetUartDescription();
void PrintUartInitResult();

#endif /* UART_INIT_H */
