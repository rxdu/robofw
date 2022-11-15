/*
 * rccar_actuators.h
 *
 * Created on: Jan 31, 2022 22:43
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef ACTUATOR_RCCAR_ACTUATORS_H
#define ACTUATOR_RCCAR_ACTUATORS_H

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/pwm_interface.h"

typedef struct {
  PwmDescriptor *dd_pwm1;
  PwmDescriptor *dd_pwm2;
} RcCarActuatorConf;

bool InitRcCarActuators(RcCarActuatorConf *cfg);
_Noreturn void RcCarActuatorServiceLoop(void *p1, void *p2, void *p3);

#endif /* ACTUATOR_RCCAR_ACTUATORS_H */
