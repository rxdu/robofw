/*
 * hwconfig.h
 *
 * Created on: Jan 22, 2022 22:22
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef HWCONFIG_H
#define HWCONFIG_H

#include "stdbool.h"

#include "mcal/interface/dio_interface.h"
#include "mcal/interface/led_interface.h"
#include "mcal/interface/pwm_interface.h"
#include "mcal/interface/uart_interface.h"
#include "mcal/interface/can_interface.h"
#include "mcal/interface/encoder_interface.h"

bool InitHardware();

#endif /* HWCONFIG_H */
