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

#include "sbus/sbus.h"

static SbusInstance sbus_decoder;

#define SBUS_CHN_MAX 1850
#define SBUS_CHN_MID 1023
#define SBUS_CHN_MIN 240

bool InitSbus(SbusConf* cfg) {
  SbusDecoderInit(&sbus_decoder);

  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  bool ret = false;
  ret = ConfigureUart(cfg->dd_uart, sbus_cfg);
  if (!ret) {
    printk("[ERROR] Failed to configure UART for SBUS\n");
  }

  ret = SetupUartAsyncMode(cfg->dd_uart);
  if (!ret) {
    printk("[ERROR] Failed to setup async mode for UART of SBUS\n");
  }

  StartUartAsyncReceive(cfg->dd_uart);

  return true;
}

void UpdateSbus(void* p1) {
  static SbusMessage sbus_msg;

  ReceiverServiceConf* cfg = (ReceiverServiceConf*)p1;
  SbusConf* sbus_cfg = (SbusConf*)(cfg->rcvr_cfg);

  if (k_sem_take(&(sbus_cfg->dd_uart->rx_sem), K_FOREVER) == 0) {
    uint8_t ch;
    while (ring_buf_get(&sbus_cfg->dd_uart->ring_buffer, &ch, 1) != 0) {
      //   printk("here received a msg\n");

      if (SbusDecodeMessage(&sbus_decoder, ch, &sbus_msg)) {
        for (int i = 0; i < RECEIVER_CHANNEL_NUMBER; ++i) {
          cfg->receiver_data.channels[i] =
              (sbus_msg.channels[i] - SBUS_CHN_MID) * 1.0f /
              (SBUS_CHN_MAX - SBUS_CHN_MID);
        }
        // printk("%04d %04d %04d %04d, %04d %04d %04d %04d\n",
        //        sbus_msg.channels[0], sbus_msg.channels[1], sbus_msg.channels[2],
        //        sbus_msg.channels[3], sbus_msg.channels[4], sbus_msg.channels[5],
        //        sbus_msg.channels[6], sbus_msg.channels[7]);
        while (k_msgq_put(cfg->msgq_out, &cfg->receiver_data, K_NO_WAIT) != 0) {
          k_msgq_purge(cfg->msgq_out);
        }
      }
    }
  }
}