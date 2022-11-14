/*
 * rccar_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "rccar_interface.h"

#include "mcal/hwconfig.h"

#include "receiver/receiver_service.h"
#include "actuator/actuator_service.h"
// #include "coordinator/coordinator_service.h"

#include "actuator/tbot_actuators.h"

#define RCCAR_LED_STATUS DD_LED0
#define RCCAR_LED_USER1 DD_LED1

#define RCCAR_PWM1 DD_PWM0
#define RCCAR_PWM2 DD_PWM1

// rc input
#define RCCAR_UART_SBUS DD_UART0

// CAN uplink/downlink
#define RCCAR_CAN_UPLINK DD_CAN0

// Negative prio threads will not be pre-empted
#define TASK_PRIORITY_VIP -1
#define TASK_PRIORITY_HIGHEST 1
#define TASK_PRIORITY_HIGH 2
#define TASK_PRIORITY_MID 3
#define TASK_PRIORITY_LOW 4

K_MSGQ_DEFINE(receiver_data_queue, sizeof(ReceiverData), 1, 8);
K_MSGQ_DEFINE(actuator_data_queue, sizeof(ActuatorCmd), 16, 8);
// K_MSGQ_DEFINE(encoder_rpm_queue, sizeof(EstimatedSpeed), 1, 8);
// K_MSGQ_DEFINE(desired_rpm_queue, sizeof(DesiredRpm), 1, 8);
// K_MSGQ_DEFINE(speed_control_feedback_queue, sizeof(SpeedControlFeedback), 1,
// 8); K_MSGQ_DEFINE(desired_motion_queue, sizeof(DesiredMotion), 1, 8);
// K_MSGQ_DEFINE(motion_control_feedback_queue, sizeof(MotionControlFeedback),
// 1,
//               8);
K_MSGQ_DEFINE(robot_state_queue, sizeof(RobotState), 1, 8);
// K_MSGQ_DEFINE(can_desired_motion_queue, sizeof(DesiredMotion), 1, 8);
K_MSGQ_DEFINE(supervisor_cmd_queue, sizeof(SupervisorCommand), 1, 8);

static ReceiverServiceDef rcvr_srv;
static ActuatorServiceDef actr_srv;

bool InitRobot() {
  // load all drivers from device tree
  if (!InitHardware()) return false;

  bool ret = false;
  (void)ret;

  // configure drivers required by robot
  // LED for debugging
  TurnOnLed(GetLedDescriptor(RCCAR_LED_STATUS));
  TurnOffLed(GetLedDescriptor(RCCAR_LED_USER1));

  // receiver service
  rcvr_srv.tconf.priority = TASK_PRIORITY_HIGHEST;
  rcvr_srv.tconf.delay_ms = 0;
  rcvr_srv.tconf.period_ms = 7;

  static SbusConf sbus_cfg;
  sbus_cfg.dd_uart = GetUartDescriptor(RCCAR_UART_SBUS);

  rcvr_srv.sconf.type = RCVR_SBUS;
  rcvr_srv.sconf.rcvr_cfg = &sbus_cfg;

  rcvr_srv.sdata.rc_data_msgq = &receiver_data_queue;

  ret = StartReceiverService(&rcvr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start receiver service\n");
    return false;
  } else {
    printk("[INFO] Started receiver service\n");
  }

  // actuator service
  actr_srv.tconf.priority = TASK_PRIORITY_HIGH;
  actr_srv.tconf.delay_ms = 500;
  actr_srv.tconf.period_ms = 20;

  static TbotActuatorConf tbot_motor_cfg;
  actr_srv.sconf.type = ACTR_TBOT;
  actr_srv.sconf.active_motor_num = 2;
  actr_srv.sconf.actuator_cfg = &tbot_motor_cfg;

  actr_srv.sdata.actuator_cmd_msgq = &actuator_data_queue;

//   ret = StartActuatorService(&actr_srv);
//   if (!ret) {
//     printk("[ERROR] Failed to start actuator service\n");
//     return false;
//   } else {
//     printk("[INFO] Started actuator service\n");
//   }

  printk("-----------------------------------------------------\n");

  return true;
}

_Noreturn void ShowRobotPanic() {
  LedDescriptor *led0 = GetLedDescriptor(RCCAR_LED_STATUS);
  LedDescriptor *led1 = GetLedDescriptor(RCCAR_LED_USER1);

  k_thread_abort(rcvr_srv.tconf.tid);
  k_thread_abort(actr_srv.tconf.tid);

  TurnOnLed(led0);
  TurnOnLed(led1);

  while (1) {
    ToggleLed(led0);
    ToggleLed(led1);

    k_msleep(1000);
  }
}
