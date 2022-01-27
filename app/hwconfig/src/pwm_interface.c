/*
 * pwm_interface.c
 *
 * Created on: Jan 23, 2022 16:46
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "hwconfig/pwm_interface.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>
#include <sys/printk.h>

#define initialize_pwm(x, node_label, desc)                                 \
  {                                                                         \
    pwm_desc.descriptor[x].device =                                         \
        DEVICE_DT_GET(DT_PWMS_CTLR(DT_ALIAS(node_label)));                  \
    pwm_desc.descriptor[x].channel = DT_PWMS_CHANNEL(DT_ALIAS(node_label)); \
    pwm_desc.descriptor[x].period = DT_PWMS_PERIOD(DT_ALIAS(node_label));   \
    pwm_desc.descriptor[x].flags = DT_PWMS_FLAGS(DT_ALIAS(node_label));     \
    if (!desc.descriptor[x].device) {                                       \
      printk("[ERROR] PWM device driver not found\n");                      \
      return false;                                                         \
    }                                                                       \
    desc.descriptor[x].active = true;                                       \
  }

static PwmDescription pwm_desc;

bool InitPwm() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_PWM_NUM; ++i) pwm_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm0), okay)
  initialize_pwm(0, xpwm0, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm1), okay)
  initialize_pwm(1, xpwm1, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm2), okay)
  initialize_pwm(2, xpwm2, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm3), okay)
  initialize_pwm(3, xpwm3, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm4), okay)
  initialize_pwm(4, xpwm4, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(xpwm5), okay)
  initialize_pwm(5, xpwm5, pwm_desc);
#endif

  printk("[INFO] Initialized PWM\n");
  PrintPwmInitResult();

  return true;
}

PwmDescription* GetPwmDescription() { return &pwm_desc; }

void PrintPwmInitResult() {
  uint32_t count = 0;
  for (int i = 0; i < DD_PWM_NUM; ++i) {
    if (pwm_desc.descriptor[i].active) {
      count++;
      printk(" - [xPWM%d] %s \n", i, "active");
    }
  }
  printk(" => Number of active instances: %d\n", count);
}

void SetPwmDutyCycle(PwmList dev_id, float duty_cycle) {
  if (!pwm_desc.descriptor[dev_id].active) {
    printk("[xPWM] Device inactive\n");
    return;
  }

  if (duty_cycle < 0) duty_cycle = 0;
  if (duty_cycle > 1) duty_cycle = 1;

  pwm_pin_set_usec(pwm_desc.descriptor[dev_id].device,
                   pwm_desc.descriptor[dev_id].channel,
                   pwm_desc.descriptor[dev_id].period,
                   pwm_desc.descriptor[dev_id].period * duty_cycle,
                   pwm_desc.descriptor[dev_id].flags);
}