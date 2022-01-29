/*
 * pwm_interface.h
 *
 * Created on: Jan 23, 2022 16:45
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef PWM_INTERFACE_H
#define PWM_INTERFACE_H

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
PwmDescriptor *GetPwmDescriptor(PwmList dev_id);
void PrintPwmInitResult();

void SetPwmDutyCycle(PwmDescriptor *dd, float duty_cycle);

#endif /* PWM_INTERFACE_H */
