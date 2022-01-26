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

  if (InitLed()) {
    printk("[INFO]: Initialized LED\n");
    PrintLedInitResult();
  } else {
    printk("[ERROR]: Failed to setup LED\n");
  }

  if (InitDio()) {
    printk("[INFO]: Initialized DIO\n");
    PrintDioInitResult();
  } else {
    printk("[ERROR]: Failed to setup DIO\n");
  }

  if (InitPwm()) {
    printk("[INFO]: Initialized PWM\n");
    PrintPwmInitResult();
  } else {
    printk("[ERROR]: Failed to setup PWM\n");
  }

  if (InitUart()) {
    printk("[INFO]: Initialized UART\n");
    PrintUartInitResult();
  } else {
    printk("[ERROR]: Failed to setup UART\n");
  }

  // LED
  LedDescription* led_desc = GetLedDescription();
  TurnOnLed(&led_desc->descriptor[0]);
  TurnOffLed(&led_desc->descriptor[1]);

  // DIO
  DioDescription* dio_desc = GetDioDescription();
  ConfigureDio(&dio_desc->descriptor[0], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[1], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[2], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[3], GPIO_OUTPUT | GPIO_PULL_UP);
  ConfigureDio(&dio_desc->descriptor[4], GPIO_OUTPUT | GPIO_PULL_UP);
  SetDio(&dio_desc->descriptor[0], 1);
  SetDio(&dio_desc->descriptor[1], 1);
  SetDio(&dio_desc->descriptor[2], 1);
  SetDio(&dio_desc->descriptor[3], 1);
  SetDio(&dio_desc->descriptor[4], 1);

  // PWM
  PwmDescription* pwm_desc = GetPwmDescription();
  SetPwmDutyCycle(&pwm_desc->descriptor[0], 0.5);
  SetPwmDutyCycle(&pwm_desc->descriptor[1], 0.5);
  SetPwmDutyCycle(&pwm_desc->descriptor[2], 0.5);
  SetPwmDutyCycle(&pwm_desc->descriptor[3], 0.5);

  // UART
  UartDescription* uart_desc = GetUartDescription();

  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  ConfigureUart(&uart_desc->descriptor[0], sbus_cfg);
  SetupUartAsyncMode(&uart_desc->descriptor[0]);
  StartUartAsyncReceive(&uart_desc->descriptor[0]);

  struct uart_config uart_test_cfg;
  uart_test_cfg.baudrate = 115200;
  uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  ConfigureUart(&uart_desc->descriptor[1], uart_test_cfg);
  SetupUartAsyncMode(&uart_desc->descriptor[1]);

  ConfigureUart(&uart_desc->descriptor[2], uart_test_cfg);
  SetupUartAsyncMode(&uart_desc->descriptor[2]);
  StartUartAsyncReceive(&uart_desc->descriptor[2]);

  printk("-----------------------------------------\n");

  uint8_t count = 0;
  uint8_t data[] = "hello";
  
  (void)data;

  while (1) {
    // ToggleGpio(EN1);
    ToggleLed(&led_desc->descriptor[0]);
    ToggleLed(&led_desc->descriptor[1]);
    ToggleLed(&led_desc->descriptor[2]);

    // if (!StartUartAsyncSend(&uart_desc->descriptor[0], data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n", uart_desc->descriptor[0].device->name);
    // }
    // if (!StartUartAsyncSend(&uart_desc->descriptor[1], data, sizeof(data),
    //                         200)) {
    //   printk("%s failed to send\n",
    // uart_desc->descriptor[1].device->name);
    // }

    if (k_sem_take(&uart_desc->descriptor[0].rx_sem, K_MSEC(50)) == 0) {
      uint8_t ch;
      while (ring_buf_get(&uart_desc->descriptor[0].ring_buffer, &ch, 1) != 0) {
        printk("%02x ", ch);
      }
    }

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
