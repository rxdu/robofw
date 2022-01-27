/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/sensor.h>

#include "hwconfig/hwconfig.h"

#define SLEEP_TIME_MS 500

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  InitHardware();

  // LED
  TurnOnLed(DD_LED0);
  TurnOffLed(DD_LED1);

  // DIO
  ConfigureDio(DD_DIO0, GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(DD_DIO1, GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(DD_DIO2, GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(DD_DIO3, GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(DD_DIO4, GPIO_OUTPUT | GPIO_PULL_UP);
  SetDio(DD_DIO0, 1);
  SetDio(DD_DIO1, 1);
  SetDio(DD_DIO2, 1);
  SetDio(DD_DIO3, 1);
  SetDio(DD_DIO4, 1);

  // PWM
  SetPwmDutyCycle(DD_PWM0, 0.5);
  SetPwmDutyCycle(DD_PWM1, 0.5);
  SetPwmDutyCycle(DD_PWM2, 0.5);
  SetPwmDutyCycle(DD_PWM3, 0.5);

  // UART
  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  ConfigureUart(DD_UART0, sbus_cfg);
  SetupUartAsyncMode(DD_UART0);
  StartUartAsyncReceive(DD_UART0);

  struct uart_config uart_test_cfg;
  uart_test_cfg.baudrate = 115200;
  uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  ConfigureUart(DD_UART1, uart_test_cfg);
  SetupUartAsyncMode(DD_UART1);

  ConfigureUart(DD_UART2, uart_test_cfg);
  SetupUartAsyncMode(DD_UART2);
  StartUartAsyncReceive(DD_UART2);

  // CAN
  struct zcan_filter can_filter;
  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  can_filter.rtr = CAN_DATAFRAME;
  can_filter.rtr_mask = 1;
  can_filter.id_mask = 0;
  ConfigureCan(DD_CAN0, CAN_NORMAL_MODE, 500000, can_filter);
  ConfigureCan(DD_CAN1, CAN_NORMAL_MODE, 1000000, can_filter);

  printk("--------------------------------------------\n");

  uint8_t count = 0;
  uint8_t data[] = "hello";
  uint8_t candata[] = {0x11, 0x22, 0x55, 0x66};
  struct zcan_frame rx_frame;

  (void)data;

  CanDescription* can_desc = GetCanDescription();

  while (1) {
    // ToggleGpio(EN1);
    ToggleLed(DD_LED0);
    ToggleLed(DD_LED1);
    ToggleLed(DD_LED2);

    // if (!StartUartAsyncSend(DD_UART0, data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n", uart_desc->descriptor[0].device->name);
    // }
    // if (!StartUartAsyncSend(DD_UART1, data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n",
    // uart_desc->descriptor[1].device->name);
    // }
    // if (k_sem_take(DD_UART0.rx_sem, K_MSEC(50)) == 0) {
    //   uint8_t ch;
    //   while (ring_buf_get(DD_UART0.ring_buffer, &ch, 1) !=
    //   0) {
    //     printk("%02x ", ch);
    //   }
    // }

    if (k_msgq_get(can_desc->descriptor[0].msgq, &rx_frame, K_MSEC(50)) == 0) {
      printk("CAN1 %02x: ", rx_frame.id);
      for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ", rx_frame.data[i]);
      printk("\n");
    }
    // printk("%s sending\n", can_desc->descriptor[0].device->name);
    int ret = SendCanFrame(DD_CAN0, 0x121, true, candata, 4);
    if (ret != CAN_TX_OK) {
      printk("%s send failed: %d\n", can_desc->descriptor[0].device->name, ret);
    } else {
      //   printk("%s sent\n", can_desc->descriptor[0].device->name);
    }

    // if (k_msgq_get(can_desc->descriptor[1].msgq, &rx_frame, K_MSEC(50)) == 0)
    // {
    //   printk("CAN2 %02x: ", rx_frame.id);
    //   for (int i = 0; i < rx_frame.dlc; ++i) printk("%02x ",
    //   rx_frame.data[i]); printk("\n");
    // }
    // if (SendCanFrame(DD_CAN1, 0x121, true, candata, 4) !=
    //     CAN_TX_OK) {
    // }

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
