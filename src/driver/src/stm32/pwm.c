/*
 * pwm.c
 *
 * Created on: May 29, 2021 11:51
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "driver/interface.h"

#include <stdio.h>

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>

#define PWM_OUTPUT_PERIOD_US 20000

// #define PWM_OUTPUT_LABLE DT_LABEL(DT_ALIAS(ctrlpwm))
#define PWM_OUTPUT_LABLE DT_LABEL(DT_NODELABEL(pwm3))

typedef struct {
  const struct device* device;
  uint32_t timer_channel;
} PwmMapping;

typedef struct {
  PwmMapping channels[2];
} PwmConfig;

static PwmConfig pwm_cfg;

bool SetupPwm(void) {
  pwm_cfg.channels[SERVO_PWM].device = device_get_binding(PWM_OUTPUT_LABLE);
  pwm_cfg.channels[SERVO_PWM].timer_channel = 3;

  pwm_cfg.channels[MOTOR_PWM].device = device_get_binding(PWM_OUTPUT_LABLE);
  pwm_cfg.channels[MOTOR_PWM].timer_channel = 4;

  if (!pwm_cfg.channels[SERVO_PWM].device ||
      !pwm_cfg.channels[MOTOR_PWM].device) {
    printf("PWM: Device driver not found.\n");
    return false;
  }
  return true;
}

void SetPwmDutyCycle(PwmChannel chn, float duty_cycle) {
  if (duty_cycle < 0) duty_cycle = 0;
  if (duty_cycle > 1) duty_cycle = 1;

  pwm_pin_set_usec(pwm_cfg.channels[chn].device,
                   pwm_cfg.channels[chn].timer_channel, PWM_OUTPUT_PERIOD_US,
                   PWM_OUTPUT_PERIOD_US * duty_cycle,
                   PWM_CAPTURE_MODE_CONTINUOUS | PWM_CAPTURE_TYPE_BOTH);
}