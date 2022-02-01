/*
 * sbus_receiver.h
 *
 * Created on: Jan 29, 2022 15:28
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef SBUS_RECEIVER_H
#define SBUS_RECEIVER_H

#include "mcal/interface/dio_interface.h"
#include "mcal/interface/uart_interface.h"

typedef struct {
  UartDescriptor* dd_uart;
  DioDescriptor* dd_dio_inv;
} SbusConf;

bool InitSbus(SbusConf* cfg);
void UpdateSbus(void* p1);

#endif /* SBUS_RECEIVER_H */
