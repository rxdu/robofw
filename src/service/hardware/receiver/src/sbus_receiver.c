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

#define SBUS_CHN_MAX 1807
#define SBUS_CHN_MID 1023
#define SBUS_CHN_MIN 240

bool InitSbus(SbusConf *cfg) {
  SbusDecoderInit(&cfg->sbus_decoder);

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

void SbusReceiverServiceLoop(void *p1, void *p2, void *p3) {
  ReceiverServiceDef *cfg = (ReceiverServiceDef *) p1;
  SbusConf *sbus_cfg = (SbusConf *) (cfg->sconf.rcvr_cfg);

  while (1) {
    if (k_sem_take(&(sbus_cfg->dd_uart->rx_sem), K_FOREVER) == 0) {
      uint8_t ch;
      while (ring_buf_get(&sbus_cfg->dd_uart->ring_buffer, &ch, 1) != 0) {
        //   printk("here received a sbus msg\n");
        //   printk("processing: %x\n", (int)ch);
        if (SbusDecodeMessage(&sbus_cfg->sbus_decoder, ch, &sbus_cfg->sbus_msg_buffer)) {
          for (int i = 0; i < RECEIVER_CHANNEL_NUMBER; ++i) {
            if (cfg->sdata.receiver_data.channels[i] > SBUS_CHN_MID) {
              cfg->sdata.receiver_data.channels[i] = (sbus_cfg->sbus_msg_buffer.channels[i] - SBUS_CHN_MID) * 1.0f /
                  (SBUS_CHN_MAX - SBUS_CHN_MID);
            } else {
              cfg->sdata.receiver_data.channels[i] = (sbus_cfg->sbus_msg_buffer.channels[i] - SBUS_CHN_MID) * 1.0f /
                  (SBUS_CHN_MID - SBUS_CHN_MIN);
            }
          }
          while (k_msgq_put(cfg->interface.rc_data_msgq_out, &cfg->sdata.receiver_data, K_NO_WAIT) != 0) {
            k_msgq_purge(cfg->interface.rc_data_msgq_out);
          }
          printk("sbus: %04d %04d %04d %04d, %04d %04d %04d %04d\n",
                 sbus_cfg->sbus_msg_buffer.channels[0],
                 sbus_cfg->sbus_msg_buffer.channels[1],
                 sbus_cfg->sbus_msg_buffer.channels[2],
                 sbus_cfg->sbus_msg_buffer.channels[3],
                 sbus_cfg->sbus_msg_buffer.channels[4],
                 sbus_cfg->sbus_msg_buffer.channels[5],
                 sbus_cfg->sbus_msg_buffer.channels[6],
                 sbus_cfg->sbus_msg_buffer.channels[7]);
        }
      }
    }
  }
}