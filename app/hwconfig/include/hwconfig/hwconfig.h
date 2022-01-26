/*
 * hwconfig.h
 *
 * Created on: Jan 22, 2022 22:22
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef HWCONFIG_H
#define HWCONFIG_H

#ifdef HWCONFIG_ENABLE_ALL
#define HWCONFIG_ENABLE_DIO 1
#define HWCONFIG_ENABLE_LED 1
#define HWCONFIG_ENABLE_PWM 1
#define HWCONFIG_ENABLE_UART 1
#define HWCONFIG_ENABLE_CAN 1
#endif

#ifdef HWCONFIG_ENABLE_DIO
#include "hwconfig/dio_interface.h"
#endif

#ifdef HWCONFIG_ENABLE_LED
#include "hwconfig/led_interface.h"
#endif

#ifdef HWCONFIG_ENABLE_PWM
#include "hwconfig/pwm_interface.h"
#endif

#ifdef HWCONFIG_ENABLE_UART
#include "hwconfig/uart_interface.h"
#endif

#ifdef HWCONFIG_ENABLE_CAN
#include "hwconfig/can_interface.h"
#endif

bool InitHardware();

#endif /* HWCONFIG_H */
