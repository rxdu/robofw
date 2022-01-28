/*
 * tbot_interface.h
 *
 * Created on: Jan 27, 2022 21:53
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef TBOT_INTERFACE_H
#define TBOT_INTERFACE_H

#include "robot/tbot/tbot_driver.h"

bool InitRobot();

void TurnOnLight();
void TurnOffLight();

void SetMotorCmd(float left, float right);

#endif /* TBOT_INTERFACE_H */
