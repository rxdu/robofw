/*
 * vesc_cmd_parser.h
 *
 * Created on: Nov 23, 2022 23:09
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef VESC_VESC_CMD_PARSER_H
#define VESC_VESC_CMD_PARSER_H

#include "vesc/vesc_frame.h"
#include "vesc/vesc_cmd_packet.h"

VescFrame VescSetServoPosCmdPacketToFrame(uint8_t vesc_id, float pos);
VescFrame VescSetDutyCycleCmdPacketToFrame(uint8_t vesc_id, float duty);
VescFrame VescSetCurrentCmdPacketToFrame(uint8_t vesc_id, float current);
VescFrame VescSetCurrentBrakeCmdPacketToFrame(uint8_t vesc_id, float current);
VescFrame VescSetRpmCmdPacketToFrame(uint8_t vesc_id, int32_t rpm);
VescFrame VescSetPositionCmdPacketToFrame(uint8_t vesc_id, float pos);

VescCmdPacket VescFrameToCmdPacket(VescFrame* frame);

#endif /* VESC_VESC_CMD_PARSER_H */
