/*
 * pwm_init.c
 *
 * Created on: Jan 23, 2022 13:42
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "hwconfig/pwm_init.h"

#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>
#include <sys/printk.h>

#define initialize_pwm(x, node_label, desc)                                   \
  {                                                                           \
    pwm_desc.descriptor[x].device =                                           \
        DEVICE_DT_GET(DT_PWMS_CTLR(DT_NODELABEL(node_label)));                \
    pwm_desc.descriptor[x].channel =                                          \
        DT_PWMS_CHANNEL(DT_NODELABEL(node_label));                            \
    pwm_desc.descriptor[x].period = DT_PWMS_PERIOD(DT_NODELABEL(node_label)); \
    pwm_desc.descriptor[x].flags = DT_PWMS_FLAGS(DT_NODELABEL(node_label));   \
    if (!desc.descriptor[x].device) {                                         \
      printk("PWM: Device driver not found.\n");                              \
      return false;                                                           \
    }                                                                         \
    desc.descriptor[x].active = true;                                         \
  }

static PwmDescription pwm_desc;

bool InitPwm() {
  // disable all by default, enable only if successfully initialized below
  for (int i = 0; i < DD_PWM_NUM; ++i) pwm_desc.descriptor[i].active = false;

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm0), okay)
  initialize_pwm(0, dd_pwm0, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm1), okay)
  initialize_pwm(1, dd_pwm1, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm2), okay)
  initialize_pwm(2, dd_pwm2, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm3), okay)
  initialize_pwm(3, dd_pwm3, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm4), okay)
  initialize_pwm(4, dd_pwm4, pwm_desc);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dd_pwm5), okay)
  initialize_pwm(5, dd_pwm5, pwm_desc);
#endif

  return true;
}

PwmDescription* GetPwmDescription() { return &pwm_desc; }

void PrintPwmInitResult() {
  for (int i = 0; i < DD_PWM_NUM; ++i) {
    if (pwm_desc.descriptor[i].active) {
      printk(" - [xPWM%d]: %s \n", i, "active");
    }
  }
}