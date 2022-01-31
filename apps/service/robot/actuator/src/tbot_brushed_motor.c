/*
 * tbot_brushed_motor.c
 *
 * Created on: Jan 31, 2022 22:45
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "actuator/tbot_brushed_motor.h"

#include "actuator/actuator_service.h"

bool InitTbotBrushedMotor(TbotBrushedMotorConf* cfg) {
  ConfigureDio(cfg->dd_en1, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_dir1, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_en2, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_dir2, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(cfg->dd_en1, 0);
  SetDio(cfg->dd_dir1, 0);
  SetDio(cfg->dd_en2, 0);
  SetDio(cfg->dd_dir2, 0);

  SetPwmDutyCycle(cfg->dd_pwm1, 0.0);
  SetPwmDutyCycle(cfg->dd_pwm2, 0.0);

  return true;
}

void UpdateTbotBrushedMotor(void* p1) {
  ActuatorServiceConf* cfg = (ActuatorServiceConf*)p1;
}