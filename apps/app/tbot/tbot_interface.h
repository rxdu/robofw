/*
 * tbot_interface.h
 *
 * Created on: Jan 27, 2022 21:53
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef TBOT_INTERFACE_H
#define TBOT_INTERFACE_H

#include "mcal/hwconfig.h"
#include "receiver/receiver_service.h"

#define TBOT_LED_STATUS DD_LED0
#define TBOT_LED_USER1 DD_LED1
#define TBOT_LED_USER2 DD_LED2

// left motor
#define TBOT_DIO_EN1 DD_DIO0
#define TBOT_DIO_DIR1 DD_DIO1
#define TBOT_PWM1 DD_PWM0

// right motor
#define TBOT_DIO_EN2 DD_DIO2
#define TBOT_DIO_DIR2 DD_DIO3
#define TBOT_PWM2 DD_PWM1

// front light
#define TBOT_DIO_LIGHT_CTRL DD_DIO4

// rc input
#define TBOT_UART_SBUS DD_UART0

// gps receiver
#define TBOT_UART_GPS DD_UART1

// ultrasonic sensor
#define TBOT_UART_ULTRASONIC DD_UART2

// CAN uplink/downlink
#define TBOT_CAN_UPLINK DD_CAN0
#define TBOT_CAN_DOWNLINK DD_CAN1

typedef struct {
  LedDescription* leds;
  DioDescription* dios;
  PwmDescription* pwms;
  UartDescription* uarts;
  CanDescription* cans;
} RobotHardware;

typedef struct {
  ReceiverServiceConf rcvr_srv;
} RobotService;

bool InitRobot();

RobotHardware* GetHardware();
RobotService* GetService();

void TurnOnLight();
void TurnOffLight();

void SetMotorCmd(float left, float right);

void StartTasks();

#endif /* TBOT_INTERFACE_H */
