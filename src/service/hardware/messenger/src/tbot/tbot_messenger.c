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

#define TBOT_ENCODER_RAW_DATA_CAN_ID 0x211
#define TBOT_ENCODER_FILTERED_DATA_CAN_ID 0x212

void EncodeCanMessage(const TbotMsg *msg, struct zcan_frame *frame) {
  switch (msg->type) {
    case kTbotEncoderRawData: {
      frame->id = TBOT_ENCODER_RAW_DATA_CAN_ID;
      frame->dlc = 8;
      frame->data[0] =
          (uint8_t) ((msg->data.encoder_raw_data.left & 0xff000000) >> 24);
      frame->data[1] =
          (uint8_t) ((msg->data.encoder_raw_data.left & 0x00ff0000) >> 16);
      frame->data[2] =
          (uint8_t) ((msg->data.encoder_raw_data.left & 0x0000ff00) >> 8);
      frame->data[3] =
          (uint8_t) ((msg->data.encoder_raw_data.left & 0x000000ff) >> 0);
      frame->data[4] =
          (uint8_t) ((msg->data.encoder_raw_data.right & 0xff000000) >> 24);
      frame->data[5] =
          (uint8_t) ((msg->data.encoder_raw_data.right & 0x00ff0000) >> 16);
      frame->data[6] =
          (uint8_t) ((msg->data.encoder_raw_data.right & 0x0000ff00) >> 8);
      frame->data[7] =
          (uint8_t) ((msg->data.encoder_raw_data.right & 0x000000ff) >> 0);
      break;
    }
    case kTbotEncoderFilteredData: {
      frame->id = TBOT_ENCODER_FILTERED_DATA_CAN_ID;
      frame->dlc = 8;
      frame->data[0] =
          (uint8_t) ((msg->data.encoder_filtered_data.left & 0xff000000) >> 24);
      frame->data[1] =
          (uint8_t) ((msg->data.encoder_filtered_data.left & 0x00ff0000) >> 16);
      frame->data[2] =
          (uint8_t) ((msg->data.encoder_filtered_data.left & 0x0000ff00) >> 8);
      frame->data[3] =
          (uint8_t) ((msg->data.encoder_filtered_data.left & 0x000000ff) >> 0);
      frame->data[4] =
          (uint8_t) ((msg->data.encoder_filtered_data.right & 0xff000000) >> 24);
      frame->data[5] =
          (uint8_t) ((msg->data.encoder_filtered_data.right & 0x00ff0000) >> 16);
      frame->data[6] =
          (uint8_t) ((msg->data.encoder_filtered_data.right & 0x0000ff00) >> 8);
      frame->data[7] =
          (uint8_t) ((msg->data.encoder_filtered_data.right & 0x000000ff) >> 0);
      break;
    }
    default: {
      break;
    }
  }
}

bool DecodeCanMessage(const struct zcan_frame *frame, TbotMsg *msg) {
  bool ret = true;
  switch (frame->id) {
    case TBOT_PWM_CMD_CAN_ID: {
      msg->type = kTbotPwmCommand;
      msg->data.pwm_cmd.pwm_left = (int8_t) (frame->data[0]);
      msg->data.pwm_cmd.pwm_right = (int8_t) (frame->data[1]);
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