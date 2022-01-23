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

#define PWM_OUTPUT_PERIOD_US 100     // 10K

#define PWM_OUTPUT1_LABEL DT_LABEL(DT_ALIAS(ctrlpwm1))
#define PWM_OUTPUT2_LABEL DT_LABEL(DT_ALIAS(ctrlpwm2))

// #define PWM_OUTPUT_LABLE DT_LABEL(DT_NODELABEL(pwm3))

typedef struct {
  const struct device* device;
  uint32_t channel;
} PWMChnDef;

typedef struct {
  PWMChnDef channels[2];
} PWMChnMapping;

static PWMChnMapping pwm_cfg;

bool SetupPwm(void) {
  pwm_cfg.channels[PWM1].device = device_get_binding(PWM_OUTPUT1_LABEL);
  pwm_cfg.channels[PWM1].channel = 1;

  pwm_cfg.channels[PWM2].device = device_get_binding(PWM_OUTPUT2_LABEL);
  pwm_cfg.channels[PWM2].channel = 3;

  if (!pwm_cfg.channels[PWM1].device || !pwm_cfg.channels[PWM2].device) {
    printf("PWM: Device driver not found.\n");
    return false;
  }
  return true;
}

void SetPwmDutyCycle(PwmChannel chn, float duty_cycle) {
  if (duty_cycle < 0) duty_cycle = 0;
  if (duty_cycle > 1) duty_cycle = 1;

  pwm_pin_set_usec(pwm_cfg.channels[chn].device, pwm_cfg.channels[chn].channel,
                   PWM_OUTPUT_PERIOD_US, PWM_OUTPUT_PERIOD_US * duty_cycle,
                   PWM_CAPTURE_MODE_CONTINUOUS | PWM_CAPTURE_TYPE_BOTH);
}