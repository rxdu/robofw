/*
 * sbus_receiver.h
 *
 * Created on: Jan 29, 2022 15:28
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef SBUS_RECEIVER_H
#define SBUS_RECEIVER_H

#include "mcal/interface/dio_interface.h"
#include "mcal/interface/uart_interface.h"

#include "sbus/sbus.h"

typedef struct {
  UartDescriptor* dd_uart;
  DioDescriptor* dd_dio_inv;
  SbusDecoderInstance sbus_decoder;
  SbusMessage sbus_msg_buffer;
} SbusConf;

bool InitSbus(SbusConf* cfg);
void SbusReceiverServiceLoop(void *p1, void *p2, void *p3);

#endif /* SBUS_RECEIVER_H */
