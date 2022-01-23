/*
 * interface.h
 *
 * Created on: Feb 17, 2021 12:26
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef TARGET_NATIVE_POSIX
#include "port/native_posix.h"
#else
#include "port/stm32.h"
#endif

/// LED
typedef enum { USER_LED1 = 0, USER_LED2 } LedChannel;

bool SetupLed(void);
void TurnOnLed(LedChannel led_num);
void TurnOffLed(LedChannel led_num);
void ToggleLed(LedChannel led_num);

/// GPIO
typedef enum { EN1, DIR1, EN2, DIR2 } GpioChannel;

bool SetupGpio(void);
void SetGpio(GpioChannel gpio_num, uint8_t value);
void ToggleGpio(GpioChannel gpio_num);

/// CAN Bus
typedef enum { DOWNLINK_CAN = 0, UPLINK_CAN } CanChannel;

bool SetupCan(CanChannel chn, CANMode mode, uint32_t baudrate);
CANMsgQueue* GetCanMessageQueue(CanChannel chn);
int SendCanFrame(CanChannel chn, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc);

/// PWM
typedef enum { PWM1 = 0, PWM2 } PwmChannel;

bool SetupPwm(void);
void SetPwmDutyCycle(PwmChannel chn, float duty_cycle);

#endif /* INTERFACE_H */
