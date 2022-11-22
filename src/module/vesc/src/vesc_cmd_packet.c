/*
 * vesc_cmd_packet.c
 *
 * Created on 7/1/22 9:04 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "vesc/vesc_cmd_packet.h"

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */

#define CAN_SFF_ID_BITS 11
#define CAN_EFF_ID_BITS 29

// CAN id
static const uint32_t VescDutyCycleCmdFrameId = 0x00000000;
static const uint32_t VescCurrentCmdFrameId = 0x00000100;

static const uint32_t VescCurrentBrakeCmdFrameId = 0x00000200;
static const uint32_t VescRpmCmdFrameId = 0x00000300;
static const uint32_t VescPositionCmdFrameId = 0x00000400;

static const uint32_t VescProcessShortBufferCmdFrameId = 0x00000800;

static const uint8_t VescCommSetServoPosId = 0x0c;

static void ClampCommand(float *cmd, float min, float max);

void ClampCommand(float *cmd, float min, float max) {
  if (*cmd > max) *cmd = max;
  if (*cmd < min) *cmd = min;
}

VescFrame VescSetServoPosCmdPacket(uint8_t vesc_id, float pos) {
  VescFrame frame_;

  // limit command
  ClampCommand(&pos, 0.0f, 1.0f);

  // convert to can frame
  frame_.can_id = VescProcessShortBufferCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 5;
  frame_.data[0] = vesc_id;
  frame_.data[1] = 0x00;  // command type: send
  frame_.data[2] = VescCommSetServoPosId;

  int16_t pos_cmd = (int16_t)(pos * 1000);
  frame_.data[3] = ((uint16_t)pos_cmd) & 0xff00 >> 8;
  frame_.data[4] = ((uint16_t)pos_cmd) & 0x00ff;

  return frame_;
}

VescFrame VescSetDutyCycleCmdPacket(uint8_t vesc_id, float duty) {
  VescFrame frame_;

  // limit command
  ClampCommand(&duty, 0.0f, 1.0f);

  // convert to can frame
  frame_.can_id = VescDutyCycleCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 4;
  int32_t duty_cmd = (int32_t)(duty * 100000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetCurrentCmdPacket(uint8_t vesc_id, float current) {
  VescFrame frame_;

  // limit command
  ClampCommand(&current, 0.0f, 20.0f);

  // convert to can frame
  frame_.can_id = VescCurrentCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 4;
  int32_t duty_cmd = (int32_t)(current * 1000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetCurrentBrakeCmdPacket(uint8_t vesc_id, float current) {
  VescFrame frame_;

  // limit command
  ClampCommand(&current, 0.0f, 20.0f);

  // convert to can frame
  frame_.can_id = VescCurrentBrakeCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 4;
  int32_t duty_cmd = (int32_t)(current * 1000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetRpmCmdPacket(uint8_t vesc_id, int32_t rpm) {
  VescFrame frame_;

  // convert to can frame
  frame_.can_id = VescRpmCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 4;
  frame_.data[0] = (rpm & 0xff000000) >> 24;
  frame_.data[1] = (rpm & 0x00ff0000) >> 16;
  frame_.data[2] = (rpm & 0x0000ff00) >> 8;
  frame_.data[3] = (rpm & 0x000000ff);

  return frame_;
}

VescFrame VescSetPositionCmdPacket(uint8_t vesc_id, float pos) {
  VescFrame frame_;

  // limit command
  ClampCommand(&pos, 0.0f, 1.0f);

  // convert to can frame
  frame_.can_id = VescPositionCmdFrameId | vesc_id | CAN_EFF_FLAG;
  frame_.can_dlc = 4;
  int32_t pos_cmd = (int32_t)(pos * 100000.0f);
  frame_.data[0] = (pos_cmd & 0xff000000) >> 24;
  frame_.data[1] = (pos_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (pos_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (pos_cmd & 0x000000ff);

  return frame_;
}
