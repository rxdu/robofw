/*
 * hwconfig.c
 *
 * Created on: Jan 26, 2022 21:35
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "mcal/hwconfig.h"

bool InitHardware() {
  bool dio_ret = InitDio();
  bool led_ret = InitLed();
  bool pwm_ret = InitPwm();
  bool uart_ret = InitUart();
  bool can_ret = InitCan();
  bool encoder_ret = InitEncoder();

  return (led_ret && dio_ret && pwm_ret && uart_ret && can_ret && encoder_ret);
}