/*
 * pwm_interface.c
 *
 * Created on: Jan 23, 2022 16:46
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/pwm_interface.h"

void SetPwmDutyCycle(PwmDescriptor* dd, float duty_cycle) {
  if (!dd->active) {
    printk("[xPWM]: device inactive\n");
    return;
  }

  if (duty_cycle < 0) duty_cycle = 0;
  if (duty_cycle > 1) duty_cycle = 1;

  pwm_pin_set_usec(dd->device, dd->channel, dd->period, dd->period * duty_cycle,
                   dd->flags);
}