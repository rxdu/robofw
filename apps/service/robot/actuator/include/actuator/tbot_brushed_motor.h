/*
 * tbot_brushed_motor.h
 *
 * Created on: Jan 31, 2022 22:43
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef TBOT_BRUSHED_MOTOR_H
#define TBOT_BRUSHED_MOTOR_H

#include <zephyr.h>
#include <device.h>

#include "mcal/interface/dio_interface.h"
#include "mcal/interface/pwm_interface.h"

typedef struct {
  DioDescriptor* dd_en1;
  DioDescriptor* dd_dir1;
  DioDescriptor* dd_en2;
  DioDescriptor* dd_dir2;

  PwmDescriptor* dd_pwm1;
  PwmDescriptor* dd_pwm2;
} TbotBrushedMotorConf;

bool InitTbotBrushedMotor(TbotBrushedMotorConf* cfg);
void UpdateTbotBrushedMotor(void* p1);

#endif /* TBOT_BRUSHED_MOTOR_H */
