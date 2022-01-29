/*
 * sbus_receiver.c
 *
 * Created on: Jan 29, 2022 20:04
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "receiver/sbus_receiver.h"
#include "receiver/receiver_service.h"

bool InitSbus(SbusConf* cfg) {
  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  bool ret = false;
  ret = ConfigureUart(cfg->dev_id, sbus_cfg);
  if (!ret) {
    printk("[ERROR] Failed to configure UART for SBUS\n");
  }

  ret = SetupUartAsyncMode(cfg->dev_id);
  if (!ret) {
    printk("[ERROR] Failed to setup async mode for UART of SBUS\n");
  }

  StartUartAsyncReceive(cfg->dev_id);

  return true;
}

void UpdateSbus(void* p1) {
  ReceiverServiceConf* cfg = (ReceiverServiceConf*)p1;
  UartDescriptor* dd = GetUartDescriptor(((SbusConf*)(cfg->rcvr_cfg))->dev_id);
  if (k_sem_take(&dd->rx_sem, K_FOREVER) == 0) {
    uint8_t ch;
    // while (ring_buf_get(DD_UART0.ring_buffer, &ch, 1) != 0) {
    //   printk("%02x ", ch);
    // }
    printk("UART msg received\n");
  }
}