/*
 * pid_controller.c
 *
 * Created on 4/22/22 9:31 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "pid_controller.h"

#include <zephyr.h>

void InitPidController(PidControllerInstance* inst) {
  inst->integral = 0;
  inst->error_last = 0;
}

float UpdatePidController(PidControllerInstance* inst, float reference,
                          float measurement) {
  float error = reference - measurement;
  float derivative = (error - inst->error_last) / inst->ts;
  float new_integral = inst->integral + error * inst->ts;

  float u =
      inst->kp * error + inst->ki * inst->integral + inst->kd * derivative;

  // anti-windup
  if (u > inst->umax) {
    u = inst->umax;
  } else if (u < -inst->umax) {
    u = -inst->umax;
  } else {
    inst->integral = new_integral;
  }

  //  printk("meas: %.4f, target: %.4f, error: %.4f, integral: %.4f, u: %.4f\n",
  //         measurement, reference, error, inst->integral, u);

  inst->error_last = error;

  return u;
}