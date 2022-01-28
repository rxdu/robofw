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

#include "hwconfig/dio_interface.h"
#include "hwconfig/led_interface.h"
#include "hwconfig/pwm_interface.h"
#include "hwconfig/uart_interface.h"
#include "hwconfig/can_interface.h"

bool InitHardware();

#endif /* HWCONFIG_H */
