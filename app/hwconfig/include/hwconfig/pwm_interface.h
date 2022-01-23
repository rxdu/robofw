/*
 * pwm_interface.h
 *
 * Created on: Jan 23, 2022 16:45
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef PWM_INTERFACE_H
#define PWM_INTERFACE_H

#include "hwconfig/pwm_init.h"

void SetPwmDutyCycle(PwmDescriptor* dd, float duty_cycle);

#endif /* PWM_INTERFACE_H */
