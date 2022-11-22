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

bool InitSbus(SbusConf *conf) {
  SbusDecoderInit(&conf->sbus_decoder);

  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  bool ret = false;
  ret = ConfigureUart(conf->dd_uart, sbus_cfg);
  if (!ret) {
    printk("[ERROR] Failed to configure UART for SBUS\n");
  }

  ret = SetupUartAsyncMode(conf->dd_uart);
  if (!ret) {
    printk("[ERROR] Failed to setup async mode for UART of SBUS\n");
  }

  StartUartAsyncReceive(conf->dd_uart);

  return true;
}

_Noreturn void SbusReceiverServiceLoop(void *p1, void *p2, void *p3) {
  ReceiverServiceDef *def = (ReceiverServiceDef *)p1;
  SbusConf *sbus_cfg = (SbusConf *)(def->sconf.rcvr_cfg);
  ReceiverData receiver_data;

  while (1) {
    if (k_sem_take(&(sbus_cfg->dd_uart->rx_sem), K_FOREVER) == 0) {
      uint8_t ch;
      while (ring_buf_get(&sbus_cfg->dd_uart->ring_buffer, &ch, 1) != 0) {
        //        printk("------> received a sbus msg\n");
        //   printk("processing: %x\n", (int)ch);
        if (SbusDecodeMessage(&sbus_cfg->sbus_decoder, ch,
                              &sbus_cfg->sbus_msg_buffer)) {
          for (int i = 0; i < RECEIVER_CHANNEL_NUMBER; ++i) {
            if (sbus_cfg->sbus_msg_buffer.channels[i] > SBUS_CHN_MID) {
              receiver_data.channels[i] =
                  (sbus_cfg->sbus_msg_buffer.channels[i] - SBUS_CHN_MID) *
                  1.0f / (SBUS_CHN_MAX - SBUS_CHN_MID);
            } else {
              receiver_data.channels[i] =
                  (sbus_cfg->sbus_msg_buffer.channels[i] - SBUS_CHN_MID) *
                  1.0f / (SBUS_CHN_MID - SBUS_CHN_MIN);
            }
          }
          while (k_msgq_put(def->interface.rc_data_msgq_out, &receiver_data,
                            K_NO_WAIT) != 0) {
            k_msgq_purge(def->interface.rc_data_msgq_out);
          }

          //          printk("sbus: %04d %04d %04d %04d, %04d %04d %04d %04d\n",
          //                 sbus_cfg->sbus_msg_buffer.channels[0],
          //                 sbus_cfg->sbus_msg_buffer.channels[1],
          //                 sbus_cfg->sbus_msg_buffer.channels[2],
          //                 sbus_cfg->sbus_msg_buffer.channels[3],
          //                 sbus_cfg->sbus_msg_buffer.channels[4],
          //                 sbus_cfg->sbus_msg_buffer.channels[5],
          //                 sbus_cfg->sbus_msg_buffer.channels[6],
          //                 sbus_cfg->sbus_msg_buffer.channels[7]);

          //   printk("receiver: %04f %04f %04f %04f, %04f %04f %04f %04f\n",
          //          receiver_data.channels[0], receiver_data.channels[1],
          //          receiver_data.channels[2], receiver_data.channels[3],
          //          receiver_data.channels[4], receiver_data.channels[5],
          //          receiver_data.channels[6], receiver_data.channels[7]);
        }
      }
    }

    //    k_msleep(def->tconf.period_ms);
  }
}
