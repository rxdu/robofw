/* 
 * native_posix.h
 * 
 * Created on: Apr 02, 2021 10:54
 * Description: 
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */ 

#ifndef NATIVE_POSIX_H
#define NATIVE_POSIX_H

#include <net/socket.h>
#include <net/socket_can.h>

#define BOARD_NATIVE_POSIX

typedef enum can_mode CANMode;
typedef struct k_msgq CANMsgQueue;

#endif /* NATIVE_POSIX_H */
