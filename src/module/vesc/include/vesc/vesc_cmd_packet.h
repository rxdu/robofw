/*
 * vesc_cmd_packet.hpp
 *
 * Created on 7/1/22 9:04 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef VESC_VESC_CMD_PACKET_H
#define VESC_VESC_CMD_PACKET_H

typedef enum {
  VescUnknownCmd = -1,
  VescSetServoPosCmd = 0,
  VescSetDutyCycleCmd,
  VescSetCurrentCmd,
  VescSetCurrentBrakeCmd,
  VescSetRpmCmd,
  VescSetPositionCmd
} VescCmdPacketType;

typedef struct {
  float pos;
} VescSetServoPosCmdPacket;

typedef struct {
  float duty;
} VescSetDutyCycleCmdPacket;

typedef struct {
  float current;
} VescSetCurrentCmdPacket;

typedef struct {
  float current;
} VescSetCurrentBrakeCmdPacket;

typedef struct {
  int32_t rpm;
} VescSetRpmCmdPacket;

typedef struct {
  float pos;
} VescSetPositionCmdPacket;

typedef struct {
  VescCmdPacketType type;
  union VescCmdPacketBody {
    VescSetServoPosCmdPacket servo_pos;
    VescSetDutyCycleCmdPacket duty_cycle;
    VescSetCurrentCmdPacket current;
    VescSetCurrentBrakeCmdPacket current_brake;
    VescSetRpmCmdPacket rpm;
    VescSetPositionCmdPacket position;
  } packet;
} VescCmdPacket;

#endif /* VESC_VESC_CMD_PACKET_H */
