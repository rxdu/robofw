/*
 * tbot_actuators.c
 *
 * Created on: Jan 31, 2022 22:45
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "actuator/tbot_actuators.h"

#include "actuator/actuator_service.h"

TbotActuatorConf *tbot_actr_cfg;

static void LimitCommand(float in, float *out);
static void SetMotorCmd(float left, float right);

bool InitTbotActuators(TbotActuatorConf *cfg) {
  tbot_actr_cfg = cfg;

  ConfigureDio(cfg->dd_dio_en1, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_dio_dir1, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_dio_en2, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  ConfigureDio(cfg->dd_dio_dir2, GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(cfg->dd_dio_en1, 0);
  SetDio(cfg->dd_dio_dir1, 0);
  SetDio(cfg->dd_dio_en2, 0);
  SetDio(cfg->dd_dio_dir2, 0);

  SetPwmDutyCycle(cfg->dd_pwm1, 0.0);
  SetPwmDutyCycle(cfg->dd_pwm2, 0.0);

  return true;
}

_Noreturn void TbotActuatorServiceLoop(void *p1, void *p2, void *p3) {
  ActuatorServiceDef *def = (ActuatorServiceDef *) p1;
  ActuatorCmd actuator_cmd;

  while (1) {
//    printk("actuator_cmd_msgq free: %d\n", k_msgq_num_free_get(def->sdata.actuator_cmd_msgq));
    while (k_msgq_get(def->sdata.actuator_cmd_msgq,
                      &actuator_cmd, K_FOREVER) == 0) {
      float cmd_left = actuator_cmd.motors[0];
      float cmd_right = actuator_cmd.motors[1];

//      printk("received cmd: %3f, %3f\n", cmd_left, cmd_right);

      LimitCommand(cmd_left, &cmd_left);
      LimitCommand(cmd_right, &cmd_right);

      // reverse right cmd (reversed motor installation direction)
      cmd_right = -cmd_right;

      printk("final cmd: %3f, %3f\n", cmd_left, cmd_right);
      SetMotorCmd(cmd_left, cmd_right);
    }
//    k_msleep(def->tconf.period_ms);
  }
}

void LimitCommand(float in, float *out) {
  float cmd = in;

  if (cmd > 1.0) cmd = 1.0;
  if (cmd < -1.0) cmd = -1.0;
  if (cmd > 0) {
    cmd = 1.0f - cmd;
  } else {
    cmd = -(1.0f + cmd);
  }
  if (cmd > -0.05 && cmd < 0.05) cmd = 0.0;
  *out = cmd;
}

void SetMotorCmd(float left, float right) {
  //   printk("set cmd (left/right): %d, %d\n", (int)(left * 100),
  //          (int)(right * 100));

  // valid cmd: (+-)(1-99)%
  if (left > 0.99) left = 0.99;
  if (left < -0.99) left = -0.99;
  if (left > 0 && left < 0.01) left = 0.01;
  if (left < 0 && left > -0.01) left = -0.01;

  if (right > 0.99) right = 0.99;
  if (right < -0.99) right = -0.99;
  if (right > 0 && right < 0.01) right = 0.01;
  if (right < 0 && right > -0.01) right = -0.01;

  if (left < 0) {
    left = -left;
    SetDio(tbot_actr_cfg->dd_dio_en1, 1);
    SetDio(tbot_actr_cfg->dd_dio_dir1, 1);
  } else {
    SetDio(tbot_actr_cfg->dd_dio_en1, 1);
    SetDio(tbot_actr_cfg->dd_dio_dir1, 0);
  }
  SetPwmDutyCycle(tbot_actr_cfg->dd_pwm1, left);

  if (right < 0) {
    right = -right;
    SetDio(tbot_actr_cfg->dd_dio_en2, 1);
    SetDio(tbot_actr_cfg->dd_dio_dir2, 1);
  } else {
    SetDio(tbot_actr_cfg->dd_dio_en2, 1);
    SetDio(tbot_actr_cfg->dd_dio_dir2, 0);
  }
  SetPwmDutyCycle(tbot_actr_cfg->dd_pwm2, right);
}