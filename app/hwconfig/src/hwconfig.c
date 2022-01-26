/*
 * hwconfig.c
 *
 * Created on: Jan 26, 2022 21:35
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/hwconfig.h"

bool InitHardware() {
  bool led_ret = true;
  bool dio_ret = true;
  bool pwm_ret = true;
  bool uart_ret = true;
  bool can_ret = true;

#ifdef HWCONFIG_ENABLE_DIO
  dio_ret = InitDio();
#endif

#ifdef HWCONFIG_ENABLE_LED
  led_ret = InitLed();
#endif

#ifdef HWCONFIG_ENABLE_PWM
  pwm_ret = InitPwm();
#endif

#ifdef HWCONFIG_ENABLE_UART
  uart_ret = InitUart();
#endif

#ifdef HWCONFIG_ENABLE_CAN
  can_ret = InitCan();
#endif

  return (led_ret && dio_ret && pwm_ret && uart_ret && can_ret);
}