/*
 * led_interface.h
 *
 * Created on: Jan 23, 2022 12:57
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

#include "hwconfig/led_init.h"

void TurnOnLed(LedDescriptor* dd);
void TurnOffLed(LedDescriptor* dd);
void ToggleLed(LedDescriptor* dd);

#endif /* LED_INTERFACE_H */
