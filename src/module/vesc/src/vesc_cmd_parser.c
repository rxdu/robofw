/*
 * vesc_cmd_parser.c
 *
 * Created on: Nov 23, 2022 23:09
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#include "vesc/vesc_cmd_parser.h"

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

// command id
static const uint8_t VescCommSetServoPosId = 0x0c;

static void ClampCommand(float* cmd, float min, float max);

void ClampCommand(float* cmd, float min, float max) {
  if (*cmd > max) *cmd = max;
  if (*cmd < min) *cmd = min;
}

VescFrame VescSetServoPosCmdPacketToFrame(uint8_t vesc_id, float pos) {
  VescFrame frame_;

  // limit command
  ClampCommand(&pos, 0.0f, 1.0f);

  // convert to can frame
  frame_.id =
      VescProcessShortBufferCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 5;
  frame_.data[0] = vesc_id;
  frame_.data[1] = 0x00;  // command type: send
  frame_.data[2] = VescCommSetServoPosId;

  int16_t pos_cmd = (int16_t)(pos * 1000);
  frame_.data[3] = ((uint16_t)pos_cmd & 0xff00) >> 8;
  frame_.data[4] = ((uint16_t)pos_cmd) & 0x00ff;

//   printk("servo pos: %d\n", pos_cmd);
//   printk("%x %x %x %x %x\n", frame_.data[0], frame_.data[1],
//          frame_.data[2], frame_.data[3], frame_.data[4]);

  return frame_;
}

VescFrame VescSetDutyCycleCmdPacketToFrame(uint8_t vesc_id, float duty) {
  VescFrame frame_;

  // limit command
  ClampCommand(&duty, 0.0f, 1.0f);

  // convert to can frame
  frame_.id = VescDutyCycleCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 4;
  int32_t duty_cmd = (int32_t)(duty * 100000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetCurrentCmdPacketToFrame(uint8_t vesc_id, float current) {
  VescFrame frame_;

  // limit command
  ClampCommand(&current, 0.0f, 20.0f);

  // convert to can frame
  frame_.id = VescCurrentCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 4;
  int32_t duty_cmd = (int32_t)(current * 1000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetCurrentBrakeCmdPacketToFrame(uint8_t vesc_id, float current) {
  VescFrame frame_;

  // limit command
  ClampCommand(&current, 0.0f, 20.0f);

  // convert to can frame
  frame_.id = VescCurrentBrakeCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 4;
  int32_t duty_cmd = (int32_t)(current * 1000.0f);
  frame_.data[0] = (duty_cmd & 0xff000000) >> 24;
  frame_.data[1] = (duty_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (duty_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (duty_cmd & 0x000000ff);

  return frame_;
}

VescFrame VescSetRpmCmdPacketToFrame(uint8_t vesc_id, int32_t rpm) {
  VescFrame frame_;

  // convert to can frame
  frame_.id = VescRpmCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 4;
  frame_.data[0] = (rpm & 0xff000000) >> 24;
  frame_.data[1] = (rpm & 0x00ff0000) >> 16;
  frame_.data[2] = (rpm & 0x0000ff00) >> 8;
  frame_.data[3] = (rpm & 0x000000ff);

  return frame_;
}

VescFrame VescSetPositionCmdPacketToFrame(uint8_t vesc_id, float pos) {
  VescFrame frame_;

  // limit command
  ClampCommand(&pos, 0.0f, 1.0f);

  // convert to can frame
  frame_.id = VescPositionCmdFrameId | (uint32_t)vesc_id | CAN_EFF_FLAG;
  frame_.dlc = 4;
  int32_t pos_cmd = (int32_t)(pos * 100000.0f);
  frame_.data[0] = (pos_cmd & 0xff000000) >> 24;
  frame_.data[1] = (pos_cmd & 0x00ff0000) >> 16;
  frame_.data[2] = (pos_cmd & 0x0000ff00) >> 8;
  frame_.data[3] = (pos_cmd & 0x000000ff);

  return frame_;
}

bool CanFrameToCmdPacket(const struct zcan_frame* frame, VescCmdPacket* pkt) {
  bool ret = true;
  if (frame->id == (VescProcessShortBufferCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    if (frame->data[2] == VescCommSetServoPosId) {
      pkt->type = VescSetServoPosCmd;
      pkt->packet.servo_pos.pos =
          (((uint16_t)frame->data[3]) << 8 | (uint16_t)(frame->data[4])) /
          1000.0f;
    }
  } else if (frame->id == (VescDutyCycleCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    pkt->type = VescSetDutyCycleCmd;
    pkt->packet.duty_cycle.duty = (int32_t)(((uint32_t)frame->data[0]) << 24 |
                                            ((uint32_t)frame->data[1]) << 16 |
                                            ((uint32_t)frame->data[2]) << 8 |
                                            ((uint32_t)frame->data[3]) << 0) /
                                  100000.0f;
  } else if (frame->id == (VescCurrentCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    pkt->type = VescSetCurrentCmd;
    pkt->packet.current.current = (int32_t)(((uint32_t)frame->data[0]) << 24 |
                                            ((uint32_t)frame->data[1]) << 16 |
                                            ((uint32_t)frame->data[2]) << 8 |
                                            ((uint32_t)frame->data[3]) << 0) /
                                  1000.0f;
  } else if (frame->id ==
             (VescCurrentBrakeCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    pkt->type = VescSetCurrentBrakeCmd;
    pkt->packet.current_brake.current =
        (int32_t)(((uint32_t)frame->data[0]) << 24 |
                  ((uint32_t)frame->data[1]) << 16 |
                  ((uint32_t)frame->data[2]) << 8 |
                  ((uint32_t)frame->data[3]) << 0) /
        1000.0f;
  } else if (frame->id == (VescRpmCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    pkt->type = VescSetRpmCmd;
    pkt->packet.rpm.rpm = (int32_t)(((uint32_t)frame->data[0]) << 24 |
                                    ((uint32_t)frame->data[1]) << 16 |
                                    ((uint32_t)frame->data[2]) << 8 |
                                    ((uint32_t)frame->data[3]) << 0);
  } else if (frame->id == (VescPositionCmdFrameId | MCU_ID | CAN_EFF_FLAG)) {
    pkt->type = VescSetPositionCmd;
    pkt->packet.position.pos = (int32_t)(((uint32_t)frame->data[0]) << 24 |
                                         ((uint32_t)frame->data[1]) << 16 |
                                         ((uint32_t)frame->data[2]) << 8 |
                                         ((uint32_t)frame->data[3]) << 0) /
                               100000.0f;
  } else {
    ret = false;
  }

  return ret;
}
