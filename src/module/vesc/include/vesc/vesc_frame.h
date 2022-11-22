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

typedef struct can_frame VescFrame;

#endif /* VESC_VESC_FRAME_H */
