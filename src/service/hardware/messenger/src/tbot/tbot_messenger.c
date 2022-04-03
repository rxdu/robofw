/* 
 * tbot_messenger.c
 *
 * Created on 4/3/22 10:07 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "tbot/tbot_messenger.h"

#define TBOT_PWM_CMD_CAN_ID 0x101
#define TBOT_MOTOR_CMD_CAN_ID 0x102
#define TBOT_MOTION_CMD_CAN_ID 0x103

void EncodeCanMessage(const TbotMsg *msg, struct zcan_frame *frame) {
}

bool DecodeCanMessage(const struct zcan_frame *frame, TbotMsg *msg) {
  bool ret = true;
  switch (frame->id) {
    case TBOT_PWM_CMD_CAN_ID: {
      msg->type = kTbotPwmCommand;
      msg->data.pwm_cmd.pwm_left = frame->data[0];
      msg->data.pwm_cmd.pwm_right = frame->data[1];
      break;
    }
    case TBOT_MOTOR_CMD_CAN_ID: {
      break;
    }
    case TBOT_MOTION_CMD_CAN_ID: {
      break;
    }
    default: {
      ret = false;
      printk("Unknown CAN ID for tbot\n");
    }
  }
  return ret;
}