/*
 * tbot_actuators.h
 *
 * Created on: Jan 31, 2022 22:43
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef TBOT_ACTUATORS_H
#define TBOT_ACTUATORS_H

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/dio_interface.h"
#include "mcal/interface/pwm_interface.h"

typedef struct {
  DioDescriptor *dd_dio_en1;
  DioDescriptor *dd_dio_dir1;
  DioDescriptor *dd_dio_en2;
  DioDescriptor *dd_dio_dir2;

  PwmDescriptor *dd_pwm1;
  PwmDescriptor *dd_pwm2;
} TbotActuatorConf;

bool InitTbotActuators(TbotActuatorConf *cfg);
_Noreturn void TbotActuatorServiceLoop(void *p1, void *p2, void *p3);

#endif /* TBOT_ACTUATORS_H */
