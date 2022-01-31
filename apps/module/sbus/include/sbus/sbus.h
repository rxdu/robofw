/*
 * sbus.h
 *
 * Created on: Feb 10, 2020 17:54
 * Description:
 *
 * 1 Start, 8 Data Bits, Even Parity, 2 Stop Bits @ 100k bps
 * Frame interval: 7ms (high-speed mode), 11ms (emulated mode)
 * Frame structure:
 *
 * [0]      0x0f
 * [1-22]   channel data [CH1-16] (22 Bytes = 22 * 8 Bits = 16 * 11 Bits)
 * [23]     [D-CH17][D-CH18][SIG_LOST][FAULT][RSV3][RSV2][RSV1][RSV0]
 * [24]     0x00
 *
 * Copyright (c) 2019 Ruixiang Du (rdu)
 */

#ifndef SBUS_H
#define SBUS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define SBUS_DMA_RX_BUFFER_SIZE 64

#define SBUS_FRAME_LENGTH 25
typedef struct {
  uint8_t decode_state;
  uint8_t data[SBUS_FRAME_LENGTH];
  size_t indexer;
} SbusDecodeBuffer;

// 16 analog + 2 digital (encoded in one byte)
#define SBUS_CHANNEL_NUMBER 18
typedef struct {
  uint16_t channels[SBUS_CHANNEL_NUMBER] __attribute__((aligned(8)));
  bool frame_loss __attribute__((aligned(8)));
  bool fault_protection __attribute__((aligned(8)));
} SbusMessage;

typedef struct {
  SbusDecodeBuffer decode_buffer;
} SbusInstance;

void SbusDecoderInit(SbusInstance *inst);
bool SbusDecodeMessage(SbusInstance *inst, uint8_t ch, SbusMessage *sbus_msg);
bool ValidateSbusMessage(const SbusMessage *sbus_msg);

#endif /* SBUS_H */
