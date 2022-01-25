/*
 * uart_interface.h
 *
 * Created on: Jan 23, 2022 22:53
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "hwconfig/uart_init.h"

void GetUartSbusConfig(struct uart_config* cfg);
bool ConfigureUart(UartDescriptor* dd, struct uart_config config);

bool SetupUartAsyncMode(UartDescriptor* dd);

void StartUartAsyncReceive(UartDescriptor* dd);
void StopUartAsyncReceive(UartDescriptor* dd);

bool StartUartAsyncSend(UartDescriptor* dd, const uint8_t* buf, size_t len,
                        int32_t timeout);

#endif /* UART_INTERFACE_H */
