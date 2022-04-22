/*
 * speed_control_service.h
 *
 * Created on: April 21, 2022 20:21
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef SPEED_CONTROL_SERVICE_H
#define SPEED_CONTROL_SERVICE_H

#include <device.h>
#include <stdbool.h>
#include <stdint.h>
#include <zephyr.h>

#include "interface/service.h"
#include "actuator/actuator_service.h"

typedef struct {
  int16_t motors[ACTUATOR_CHANNEL_NUMBER];
} __attribute__((aligned(8))) DesiredRpm;

typedef struct {
  //  LedDescriptor* dd_led_status;
} SpeedControlSrvConf;

typedef struct {
  struct k_msgq* desired_rpm_msgq;
} SpeedControlSrvData;

// struct ReceiverInterface;
struct ActuatorInterface;
struct EncoderInterface;

struct SpeedControlInterface {
  struct k_msgq* desired_rpm_msgq_in;
};

typedef struct {
  // thread config
  ThreadConfig tconf;

  // service config
  SpeedControlSrvConf sconf;
  SpeedControlSrvData sdata;

  // dependent interfaces
  struct {
    struct EncoderInterface* encoder_interface;
    struct ActuatorInterface* actuator_interface;
  } dependencies;

  // interface
  struct SpeedControlInterface interface;
} SpeedControlServiceDef;

bool StartSpeedControlService(SpeedControlServiceDef* def);

#endif /* SPEED_CONTROL_SERVICE_H */
