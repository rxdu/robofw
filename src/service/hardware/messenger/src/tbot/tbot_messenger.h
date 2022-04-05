/* 
 * tbot_messenger.h
 *
 * Created on 4/3/22 10:07 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_TBOT_TBOT_MESSENGER_H
#define ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_TBOT_TBOT_MESSENGER_H

#include <stdint.h>

#include "mcal/interface/can_interface.h"

typedef struct {
  int8_t pwm_left;
  int8_t pwm_right;
} TbotPwmCommand;

typedef struct {
  int32_t rpm_left;
  int32_t rpm_right;
} TbotMotorCommand;

typedef struct {
  int32_t linear;
  int32_t angular;
} TbotMotionCommand;

typedef enum {
  kTbotPwmCommand = 0,
  kTbotMotorCommand,
  kTbotMotionCommand
} TbotMsgType;

typedef struct {
  TbotMsgType type;
  union {
    TbotPwmCommand pwm_cmd;
  } data;
} TbotMsg;

void EncodeCanMessage(const TbotMsg *msg, struct zcan_frame *frame);
bool DecodeCanMessage(const struct zcan_frame *frame, TbotMsg *msg);

#endif //ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_TBOT_TBOT_MESSENGER_H
