/*
 * tbot_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "tbot/tbot_interface.h"

static RobotHardware robot_hardware;

bool InitRobot() {
  // load all drivers from device tree
  if (!InitHardware()) return false;

  robot_hardware.leds = GetLedDescription();
  robot_hardware.dios = GetDioDescription();
  robot_hardware.pwms = GetPwmDescription();
  robot_hardware.uarts = GetUartDescription();
  robot_hardware.cans = GetCanDescription();

  // configure drivers required by robot
  // LED for debugging
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_STATUS]);
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_USER1]);
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_USER2]);

  // motor control
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN1],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR1],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN2],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR2],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN1], 0);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR1], 0);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN2], 0);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR2], 0);

  SetPwmDutyCycle(&robot_hardware.pwms->descriptor[TBOT_PWM1], 0.0);
  SetPwmDutyCycle(&robot_hardware.pwms->descriptor[TBOT_PWM2], 0.0);

  // light control
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 0);

  // rc input
  //   struct uart_config sbus_cfg;
  //   GetUartSbusConfig(&sbus_cfg);

  //   ConfigureUart(&robot_hardware.uarts->descriptor[TBOT_UART_SBUS],
  //   sbus_cfg);
  //   SetupUartAsyncMode(&robot_hardware.uarts->descriptor[TBOT_UART_SBUS]);
  //   StartUartAsyncReceive(&robot_hardware.uarts->descriptor[TBOT_UART_SBUS]);

  //   // gps receiver
  //   struct uart_config uart_test_cfg;
  //   uart_test_cfg.baudrate = 115200;
  //   uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  //   uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  //   uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  //   uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  //   ConfigureUart(&robot_hardware.uarts->descriptor[TBOT_UART_GPS],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&robot_hardware.uarts->descriptor[TBOT_UART_GPS]);
  //   StartUartAsyncReceive(&robot_hardware.uarts->descriptor[TBOT_UART_GPS]);

  //   // ultrasonic sensor
  //   ConfigureUart(&robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC]);
  //   StartUartAsyncReceive(
  //       &robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC]);

  //   // uplink CAN to onboard computer
  //   struct zcan_filter can_filter;
  //   can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  //   can_filter.rtr = CAN_DATAFRAME;
  //   can_filter.rtr_mask = 1;
  //   can_filter.id_mask = 0;
  //   ConfigureCan(&robot_hardware.cans->descriptor[TBOT_CAN_UPLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);
  //   ConfigureCan(&robot_hardware.cans->descriptor[TBOT_CAN_DOWNLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);

  printk("----------------------------------------\n");

  return true;
}

RobotHardware* GetHardware() { return &robot_hardware; }

void TurnOnLight() {
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 1);
}

void TurnOffLight() {
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 0);
}

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
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN1], 1);
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR1], 1);
  } else {
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN1], 1);
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR1], 0);
  }
  if (left < 0) left = -left;
  SetPwmDutyCycle(&robot_hardware.pwms->descriptor[TBOT_PWM1], left);

  if (right > 0) {
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN2], 1);
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR2], 1);
  } else {
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_EN2], 1);
    SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_DIR2], 0);
  }
  if (right < 0) right = -right;
  SetPwmDutyCycle(&robot_hardware.pwms->descriptor[TBOT_PWM2], right);
}

void StartTasks() {}