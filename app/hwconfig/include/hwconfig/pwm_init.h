/*
 * pwm_init.h
 *
 * Created on: Jan 23, 2022 13:39
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef PWM_INIT_H
#define PWM_INIT_H

#include <zephyr.h>
#include <drivers/pwm.h>

typedef enum {
  DD_PWM0 = 0,
  DD_PWM1,
  DD_PWM2,
  DD_PWM3,
  DD_PWM4,
  DD_PWM5,
  DD_PWM_NUM
} PwmList;

// LED
typedef struct {
  bool active;
  const struct device* device;
  uint8_t channel;
  uint32_t period;
  pwm_flags_t flags;
} PwmDescriptor;

typedef struct {
  PwmDescriptor descriptor[DD_PWM_NUM];
} PwmDescription;

bool InitPwm();
PwmDescription* GetPwmDescription();
void PrintPwmInitResult();

#endif /* PWM_INIT_H */
