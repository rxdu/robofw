/*
 * uart_interface.c
 *
 * Created on: Jan 24, 2022 22:13
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/uart_interface.h"

void GetUartSbusConfig(struct uart_config* cfg) {
  cfg->baudrate = 100000;
  cfg->parity = UART_CFG_PARITY_EVEN;
  cfg->stop_bits = UART_CFG_STOP_BITS_2;
  cfg->data_bits = UART_CFG_DATA_BITS_8;
  cfg->flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
}

bool ConfigureUart(UartDescriptor* dd, struct uart_config config) {
  dd->config = config;
  int ret = uart_configure(dd->device, &dd->config);
  return ret == 0;
}