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
#include "board/native_posix.h"
#else
#include "board/bb_racer_cape.h"
#endif

/// LED
typedef enum { USER_LED0 = 0, USER_LED1, USER_LED2, USER_LED3 } LedIndex;

bool SetupLed(void);
void TurnOnLed(LedIndex led_num);
void TurnOffLed(LedIndex led_num);
void ToggleLed(LedIndex led_num);

/// CAN Bus
typedef enum { DOWNLINK_CAN = 0, UPLINK_CAN } CanIndex;

bool SetupCan(CanIndex chn, CANMode mode, uint32_t baudrate);
CANMsgQueue* GetCanMessageQueue(CanIndex chn);
int SendCanFrame(CanIndex chn, uint32_t id, bool is_std_id, uint8_t data[],
                 uint32_t dlc);

/// PWM
typedef enum { SERVO_PWM = 0, MOTOR_PWM } PwmIndex;
bool SetupPwm(void);
void SetPwmDutyCycle(PwmIndex chn, float duty_cycle);

#endif /* INTERFACE_H */
