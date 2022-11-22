/*
 * rccar_actuators.c
 *
 * Created on: Jan 31, 2022 22:45
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "actuator/rccar_actuators.h"

#include "actuator/actuator_service.h"

RcCarActuatorConf *rccar_actr_cfg;

static void LimitCommand(float in, float *out);
static void SetMotorCmd(float left, float right);

bool InitRcCarActuators(RcCarActuatorConf *cfg) {
  rccar_actr_cfg = cfg;

  SetPwmDutyCycle(cfg->dd_pwm1, 0.0f);
  SetPwmDutyCycle(cfg->dd_pwm2, 0.0f);

  return true;
}

_Noreturn void RcCarActuatorServiceLoop(void *p1, void *p2, void *p3) {
  ActuatorServiceDef *def = (ActuatorServiceDef *)p1;
  ActuatorCmd actuator_cmd;

  float cmd_servo = 0;
  float cmd_motor = 0;

  while (1) {
    //    printk("actuator_cmd_msgq free: %d\n",
    //    k_msgq_num_free_get(def->sdata.actuator_cmd_msgq));
    while (k_msgq_get(def->sdata.actuator_cmd_msgq, &actuator_cmd, K_FOREVER) ==
           0) {
      float cmd_servo_in = actuator_cmd.motors[0];
      float cmd_motor_in = actuator_cmd.motors[1];

      //      printk("received cmd: %3f, %3f\n", cmd_servo, cmd_motor);
      LimitCommand(cmd_servo_in, &cmd_servo);
      LimitCommand(cmd_motor_in, &cmd_motor);

      //      printk("received cmd: %3f, %3f, final cmd: %3f, %3f\n",
      //      cmd_servo_in, cmd_motor_in, cmd_servo, cmd_motor);
      SetMotorCmd(cmd_servo, cmd_motor);
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
  //   if (cmd > 0) {
  //     cmd = 1.0f - cmd;
  //   } else {
  //     cmd = -(1.0f + cmd);
  //   }
  *out = cmd;
}

void SetMotorCmd(float left, float right) {
  SetPwmDutyCycle(rccar_actr_cfg->dd_pwm1, left);
  SetPwmDutyCycle(rccar_actr_cfg->dd_pwm2, right);
}