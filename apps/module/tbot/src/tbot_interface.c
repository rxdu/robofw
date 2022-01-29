/*
 * tbot_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "tbot/tbot_interface.h"

bool InitRobot() {
  // load all drivers from device tree
  if (!InitHardware()) return false;

  // configure drivers required by robot
  // LED for debugging
  LedDescription* leds = GetLedDescription();
  TurnOffLed(leds.descriptors[TBOT_LED_STATUS]);
  TurnOffLed(leds.descriptors[TBOT_LED_USER1]);
  TurnOffLed(leds.descriptors[TBOT_LED_USER2]);

  // motor control
  DioDescription* dios = GetDioDescription();
  ConfigureDio(dios.descriptors[TBOT_DIO_EN1],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(dios.descriptors[TBOT_DIO_DIR1],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(dios.descriptors[TBOT_DIO_EN2],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(dios.descriptors[TBOT_DIO_DIR2],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(dios.descriptors[TBOT_DIO_EN1], 0);
  SetDio(dios.descriptors[TBOT_DIO_DIR1], 0);
  SetDio(dios.descriptors[TBOT_DIO_EN2], 0);
  SetDio(dios.descriptors[TBOT_DIO_DIR2], 0);

  SetPwmDutyCycle(TBOT_PWM1, 0.0);
  SetPwmDutyCycle(TBOT_PWM2, 0.0);

  // light control
  ConfigureDio(TBOT_DIO_LIGHT_CTRL, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(TBOT_DIO_LIGHT_CTRL, 0);

  // rc input
  struct uart_config sbus_cfg;
  GetUartSbusConfig(&sbus_cfg);

  ConfigureUart(TBOT_UART_SBUS, sbus_cfg);
  SetupUartAsyncMode(TBOT_UART_SBUS);
  StartUartAsyncReceive(TBOT_UART_SBUS);

  // gps receiver
  struct uart_config uart_test_cfg;
  uart_test_cfg.baudrate = 115200;
  uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  ConfigureUart(TBOT_UART_GPS, uart_test_cfg);
  SetupUartAsyncMode(TBOT_UART_GPS);
  StartUartAsyncReceive(TBOT_UART_GPS);

  // ultrasonic sensor
  ConfigureUart(TBOT_UART_ULTRASONIC, uart_test_cfg);
  SetupUartAsyncMode(TBOT_UART_ULTRASONIC);
  StartUartAsyncReceive(TBOT_UART_ULTRASONIC);

  // uplink CAN to onboard computer
  struct zcan_filter can_filter;
  can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  can_filter.rtr = CAN_DATAFRAME;
  can_filter.rtr_mask = 1;
  can_filter.id_mask = 0;
  ConfigureCan(TBOT_CAN_UPLINK, CAN_NORMAL_MODE, 500000, can_filter);
  ConfigureCan(TBOT_CAN_DOWNLINK, CAN_NORMAL_MODE, 500000, can_filter);

  printk("----------------------------------------\n");

  return true;
}

void TurnOnLight() { SetDio(TBOT_DIO_LIGHT_CTRL, 1); }

void TurnOffLight() { SetDio(TBOT_DIO_LIGHT_CTRL, 0); }

void SetMotorCmd(float left, float right) {
  printk("set cmd (left/right): %d, %d\n", (int)(left * 100),
         (int)(right * 100));

  // valid cmd: (+-)(1-99)%
  if (left > 0.99) left = 0.99;
  if (left < -0.99) left = -0.99;
  if (left > 0 && left < 0.01) left = 0.01;
  if (left < 0 && left > -0.01) left = -0.01;

  if (right > 0.99) right = 0.99;
  if (right < -0.99) right = -0.99;
  if (right > 0 && right < 0.01) right = 0.01;
  if (right < 0 && right > -0.01) right = -0.01;

  if (left > 0) {
    SetDio(TBOT_DIO_EN1, 1);
    SetDio(TBOT_DIO_DIR1, 1);
  } else {
    SetDio(TBOT_DIO_EN1, 1);
    SetDio(TBOT_DIO_DIR1, 0);
  }
  if (left < 0) left = -left;
  SetPwmDutyCycle(TBOT_PWM1, left);

  if (right > 0) {
    SetDio(TBOT_DIO_EN2, 1);
    SetDio(TBOT_DIO_DIR2, 1);
  } else {
    SetDio(TBOT_DIO_EN2, 1);
    SetDio(TBOT_DIO_DIR2, 0);
  }
  if (right < 0) right = -right;
  SetPwmDutyCycle(TBOT_PWM2, right);
}

void StartTasks() {}