/*
 * receiver_service.h
 *
 * Created on: Jan 29, 2022 15:28
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef RECEIVER_SERVICE_H
#define RECEIVER_SERVICE_H

#include <stdbool.h>

struct ReceiverApi {
  int (*GetReceiverData)(const struct ReceiverData *port);
};

typedef struct {
} ReceiverService;

bool InitReceiverService();

#endif /* RECEIVER_SERVICE_H */
