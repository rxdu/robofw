/*
 * sbus_stm32f4.c
 *
 * Created on: Aug 16, 2019 10:33
 * Description:
 *
 * Reference:
 *  [1] https://blog.csdn.net/Brendon_Tan/article/details/89854751
 *  [2] https://blog.csdn.net/yjl_programmer/article/details/88876582
 *  [3] https://blog.csdn.net/qq_31232793/article/details/80244211
 *
 * Copyright (c) 2019 Ruixiang Du (rdu)
 */

#include "sbus/sbus.h"

#include <string.h>

#define SBUS_FRAME_INTERVAL_MS 4
#define SBUS_FRAME_HEAD 0x0f
#define SBUS_FRAME_TAIL 0x00

typedef enum {
  WAIT_FOR_SOF = 0,
  WAIT_FOR_PAYLOAD,
  WAIT_FOR_EOF
} SbusDecodeState;

static void SbusBuildMessage(uint8_t *sframe_buf, SbusMessage *sbus_msg);

void SbusInit(SbusInstance *inst) {
  inst->decode_buffer.decode_state = WAIT_FOR_SOF;
}

bool SbusDecodeMessage(SbusInstance *inst, uint8_t ch, SbusMessage *sbus_msg) {
  bool new_frame_parsed = false;
  switch (inst->decode_buffer.decode_state) {
    case WAIT_FOR_SOF: {
      inst->decode_buffer.indexer = 0;
      memset(inst->decode_buffer.data, 0, SBUS_FRAME_LENGTH);
      //   DWriteString(0, "SBUS: WAIT_FOR_SOF\n");
      if (ch == SBUS_FRAME_HEAD) {
        inst->decode_buffer.decode_state = WAIT_FOR_PAYLOAD;
        inst->decode_buffer.data[inst->decode_buffer.indexer] = ch;
      }
      break;
    }
    case WAIT_FOR_PAYLOAD: {
      //   DWriteString(0, "SBUS: WAIT_FOR_PAYLOAD\n");
      inst->decode_buffer.data[++inst->decode_buffer.indexer] = ch;
      if (inst->decode_buffer.indexer == (SBUS_FRAME_LENGTH - 2))
        inst->decode_buffer.decode_state = WAIT_FOR_EOF;
      break;
    }
    case WAIT_FOR_EOF: {
      //   DWriteString(0, "SBUS: WAIT_FOR_EOF\n");
      if (ch == SBUS_FRAME_TAIL) {
        // new frame found
        new_frame_parsed = true;
        inst->decode_buffer.data[++inst->decode_buffer.indexer] = ch;
        SbusBuildMessage(inst->decode_buffer.data, sbus_msg);
        // reset state
        inst->decode_buffer.decode_state = WAIT_FOR_SOF;
      } else {  // Invalid SBUS frame
        inst->decode_buffer.decode_state = WAIT_FOR_SOF;
      }
      break;
    }
  }
  return new_frame_parsed;
}

void SbusBuildMessage(uint8_t *sframe_buf, SbusMessage *sbus_msg) {
  sbus_msg->channels[0] =
      ((int16_t)sframe_buf[1] >> 0 | ((int16_t)sframe_buf[2] << 8)) & 0x07FF;
  sbus_msg->channels[1] =
      ((int16_t)sframe_buf[2] >> 3 | ((int16_t)sframe_buf[3] << 5)) & 0x07FF;
  sbus_msg->channels[2] =
      ((int16_t)sframe_buf[3] >> 6 | ((int16_t)sframe_buf[4] << 2) |
       (int16_t)sframe_buf[5] << 10) &
      0x07FF;
  sbus_msg->channels[3] =
      ((int16_t)sframe_buf[5] >> 1 | ((int16_t)sframe_buf[6] << 7)) & 0x07FF;
  sbus_msg->channels[4] =
      ((int16_t)sframe_buf[6] >> 4 | ((int16_t)sframe_buf[7] << 4)) & 0x07FF;
  sbus_msg->channels[5] =
      ((int16_t)sframe_buf[7] >> 7 | ((int16_t)sframe_buf[8] << 1) |
       (int16_t)sframe_buf[9] << 9) &
      0x07FF;
  sbus_msg->channels[6] =
      ((int16_t)sframe_buf[9] >> 2 | ((int16_t)sframe_buf[10] << 6)) & 0x07FF;
  sbus_msg->channels[7] =
      ((int16_t)sframe_buf[10] >> 5 | ((int16_t)sframe_buf[11] << 3)) & 0x07FF;

  sbus_msg->channels[8] =
      ((int16_t)sframe_buf[12] << 0 | ((int16_t)sframe_buf[13] << 8)) & 0x07FF;
  sbus_msg->channels[9] =
      ((int16_t)sframe_buf[13] >> 3 | ((int16_t)sframe_buf[14] << 5)) & 0x07FF;
  sbus_msg->channels[10] =
      ((int16_t)sframe_buf[14] >> 6 | ((int16_t)sframe_buf[15] << 2) |
       (int16_t)sframe_buf[16] << 10) &
      0x07FF;
  sbus_msg->channels[11] =
      ((int16_t)sframe_buf[16] >> 1 | ((int16_t)sframe_buf[17] << 7)) & 0x07FF;
  sbus_msg->channels[12] =
      ((int16_t)sframe_buf[17] >> 4 | ((int16_t)sframe_buf[18] << 4)) & 0x07FF;
  sbus_msg->channels[13] =
      ((int16_t)sframe_buf[18] >> 7 | ((int16_t)sframe_buf[19] << 1) |
       (int16_t)sframe_buf[20] << 9) &
      0x07FF;
  sbus_msg->channels[14] =
      ((int16_t)sframe_buf[20] >> 2 | ((int16_t)sframe_buf[21] << 6)) & 0x07FF;
  sbus_msg->channels[15] =
      ((int16_t)sframe_buf[21] >> 5 | ((int16_t)sframe_buf[22] << 3)) & 0x07FF;

  // sbus_msg->channels[16] = sframe_buf[23];
  sbus_msg->channels[16] = sframe_buf[23] & 0x80;
  sbus_msg->channels[17] = sframe_buf[23] & 0x40;

  sbus_msg->frame_loss = sframe_buf[23] & 0x20;
  sbus_msg->fault_protection = sframe_buf[23] & 0x10;
}

bool ValidateSbusMessage(const SbusMessage *sbus_msg) {
  if (sbus_msg->fault_protection || sbus_msg->frame_loss) return false;

  uint8_t zero_num = 0;
  for (int i = 0; i < 8; ++i) {
    if (sbus_msg->channels[i] == 0) zero_num++;
    if (zero_num > 4) return false;
  }

  return true;
}