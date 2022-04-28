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
#include "interface/service.h"

typedef struct {
  SupervisedMode sup_mode;
} TbotSupervisorCommand;

typedef struct {
  int8_t pwm_left;
  int8_t pwm_right;
} TbotPwmCommand;

typedef struct {
  int32_t rpm_left;
  int32_t rpm_right;
} TbotMotorCommand;

typedef struct {
  float linear;
  float angular;
} TbotMotionCommand;

// typedef struct {
//   int32_t left;
//   int32_t right;
// } TbotEncoderRawData;

typedef struct {
  int32_t left;
  int32_t right;
} TbotSpeed;

typedef TbotSpeed TbotEncoderRawData;
typedef TbotSpeed TbotTargetRpmData;

typedef TbotEncoderRawData TbotEncoderFilteredData;

typedef enum {
  // command
  kTbotSuperviserCmmand = 0,
  kTbotPwmCommand,
  kTbotMotorCommand,
  kTbotMotionCommand,
  // feedback
  kTbotEncoderRawData,
  kTbotEncoderFilteredData,
  kTbotTargetRpmData
} TbotMsgType;

typedef struct {
  TbotMsgType type;
  union {
    // command
    TbotSupervisorCommand sup_cmd;
    TbotPwmCommand pwm_cmd;
    TbotMotorCommand rpm_cmd;
    TbotMotionCommand motion_cmd;
    // feedback
    TbotEncoderRawData encoder_raw_data;
    TbotEncoderFilteredData encoder_filtered_data;
    TbotTargetRpmData target_rpm_data;
  } data;
} TbotMsg;

void EncodeCanMessage(const TbotMsg *msg, struct zcan_frame *frame);
bool DecodeCanMessage(const struct zcan_frame *frame, TbotMsg *msg);

#endif  // ROBOFW_SRC_SERVICE_HARDWARE_MESSENGER_SRC_TBOT_TBOT_MESSENGER_H
