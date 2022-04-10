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

  SetPwmDutyCycle(cfg->dd_pwm1, -1.0);
  SetPwmDutyCycle(cfg->dd_pwm2, 1.0);

  return true;
}

_Noreturn void TbotActuatorServiceLoop(void *p1, void *p2, void *p3) {
  ActuatorServiceDef *def = (ActuatorServiceDef *) p1;
  ActuatorCmd actuator_cmd;

  float cmd_left = 0;
  float cmd_right = 0;

  while (1) {
    //    printk("actuator_cmd_msgq free: %d\n",
    //    k_msgq_num_free_get(def->sdata.actuator_cmd_msgq));
    while (k_msgq_get(def->sdata.actuator_cmd_msgq, &actuator_cmd, K_FOREVER) ==
        0) {
      float cmd_left_in = actuator_cmd.motors[0];
      float cmd_right_in = actuator_cmd.motors[1];

      //      printk("received cmd: %3f, %3f\n", cmd_left, cmd_right);
      LimitCommand(cmd_left_in, &cmd_left);
      LimitCommand(cmd_right_in, &cmd_right);

      // reverse right cmd (reversed motor installation direction)
      cmd_right = -cmd_right;

//      printk("received cmd: %3f, %3f, final cmd: %3f, %3f\n", cmd_left_in, cmd_right_in, cmd_left, cmd_right);
      SetMotorCmd(cmd_left, cmd_right);
    }
    //    k_msleep(def->tconf.period_ms);
  }
}

void LimitCommand(float in, float *out) {
  // add deadzone to avoid motion at neutral position
  if (in > -0.05 && in < 0.05) in = 0.0;

  // valid cmd: (+-)(1-99)%
  float cmd = in;
  if (cmd > 0.99) cmd = 0.99;
  if (cmd < -0.99) cmd = -0.99;
  if (cmd > 0) {
    cmd = 1.0f - cmd;
  } else {
    cmd = -(1.0f + cmd);
  }
  *out = cmd;
}

void SetMotorCmd(float left, float right) {
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