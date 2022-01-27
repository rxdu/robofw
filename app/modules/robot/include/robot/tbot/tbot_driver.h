/*
 * tbot_driver.h
 *
 * Created on: Jan 27, 2022 21:59
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef TBOT_DRIVER_H
#define TBOT_DRIVER_H

#include "hwconfig/hwconfig.h"

#define TBOT_LED_STATUS DD_LED0
#define TBOT_LED1 DD_LED1
#define TBOT_LED2 DD_LED2

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

#endif /* TBOT_DRIVER_H */
