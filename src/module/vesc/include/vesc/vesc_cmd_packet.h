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

#include "vesc/vesc_frame.h"

VescFrame VescSetServoPosCmdPacket(uint8_t vesc_id, float pos);

VescFrame VescSetDutyCycleCmdPacket(uint8_t vesc_id, float duty);

VescFrame VescSetCurrentCmdPacket(uint8_t vesc_id, float current);

VescFrame VescSetCurrentBrakeCmdPacket(uint8_t vesc_id, float current);

VescFrame VescSetRpmCmdPacket(uint8_t vesc_id, int32_t rpm);

VescFrame VescSetPositionCmdPacket(uint8_t vesc_id, float pos);

#endif /* VESC_VESC_CMD_PACKET_H */
