/*
 * vesc_frame.h
 *
 * Created on 7/1/22 9:01 PM
 * Description:
 *
 * Reference:
 * [1] https://dongilc.gitbook.io/openrobot-inc/tutorials/control-with-can
 * [2] comm/comm_can.c in https://github.com/vedderb/bldc.git
 * [3] datatypes.h in https://github.com/vedderb/bldc.git
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef VESC_VESC_FRAME_H
#define VESC_VESC_FRAME_H

#ifdef __linux__
#include <linux/can.h>
#else
#include <drivers/can.h>
#endif

#include <stdint.h>

// CAN id
static const uint32_t VescDutyCycleCmdFrameId = 0x00000000;
static const uint32_t VescCurrentCmdFrameId = 0x00000100;

static const uint32_t VescCurrentBrakeCmdFrameId = 0x00000200;
static const uint32_t VescRpmCmdFrameId = 0x00000300;
static const uint32_t VescPositionCmdFrameId = 0x00000400;

static const uint32_t VescProcessShortBufferCmdFrameId = 0x00000800;

// vesc id
#define MCU_ID ((uint32_t)0x60)
#define VESC_ID ((uint32_t)0x68)

typedef struct can_frame VescFrame;

#endif /* VESC_VESC_FRAME_H */
