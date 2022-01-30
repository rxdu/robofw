/*
 * uart_interface.h
 *
 * Created on: Jan 23, 2022 22:53
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include <zephyr.h>
#include <drivers/uart.h>
#include <sys/ring_buffer.h>

#ifndef UART_RX_BUF_SIZE
#define UART_RX_BUF_SIZE 256
#endif

#ifndef UART_RX_TIMEOUT
#define UART_RX_TIMEOUT 50
#endif

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
  struct ring_buf ring_buffer;
  // memory pre-allocated for internal use
  uint8_t double_buffer_index;
  uint8_t rx_double_buffer[2][UART_RX_BUF_SIZE];
  uint8_t ring_buffer_mem[2 * UART_RX_BUF_SIZE];
} UartDescriptor;

typedef struct {
  UartDescriptor descriptor[DD_UART_NUM];
} UartDescription;

bool InitUart();
UartDescription* GetUartDescription();
UartDescriptor* GetUartDescriptor(UartList dev_id);
void PrintUartInitResult();

void GetUartSbusConfig(struct uart_config* cfg);
bool ConfigureUart(UartDescriptor* dd, struct uart_config config);

bool SetupUartAsyncMode(UartDescriptor* dd);

void StartUartAsyncReceive(UartDescriptor* dd);
void StopUartAsyncReceive(UartDescriptor* dd);

bool StartUartAsyncSend(UartDescriptor* dd, const uint8_t* buf, size_t len,
                        int32_t timeout);

#endif /* UART_INTERFACE_H */
