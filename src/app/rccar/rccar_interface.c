/*
 * rccar_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 * 
 * Design reference:
 * [1] docs/robot/rccar/rccar_firmware.jpg
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "rccar_interface.h"

#include "mcal/hwconfig.h"

#include "receiver/receiver_service.h"
#include "actuator/actuator_service.h"
#include "messenger/messenger_service.h"
#include "coordinator/coordinator_service.h"

#include "actuator/tbot_actuators.h"
#include "motion_control/motion_control_service.h"

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
K_MSGQ_DEFINE(desired_motion_queue, sizeof(DesiredMotion), 1, 8);
K_MSGQ_DEFINE(robot_state_queue, sizeof(RobotState), 1, 8);
K_MSGQ_DEFINE(can_desired_motion_queue, sizeof(DesiredMotion), 1, 8);
K_MSGQ_DEFINE(supervisor_cmd_queue, sizeof(SupervisorCommand), 1, 8);

static ReceiverServiceDef rcvr_srv;
static ActuatorServiceDef actr_srv;
static MessengerServiceDef msger_srv;
static CoordinatorServiceDef coord_srv;
static MotionControlServiceDef motioncon_srv;

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
  //   actr_srv.tconf.priority = TASK_PRIORITY_HIGH;
  //   actr_srv.tconf.delay_ms = 500;
  //   actr_srv.tconf.period_ms = 20;

  //   static TbotActuatorConf tbot_motor_cfg;
  //   actr_srv.sconf.type = ACTR_TBOT;
  //   actr_srv.sconf.active_motor_num = 2;
  //   actr_srv.sconf.actuator_cfg = &tbot_motor_cfg;

  //   actr_srv.sdata.actuator_cmd_msgq = &actuator_data_queue;

  //   ret = StartActuatorService(&actr_srv);
  //   if (!ret) {
  //     printk("[ERROR] Failed to start actuator service\n");
  //     return false;
  //   } else {
  //     printk("[INFO] Started actuator service\n");
  //   }

  // motion control
  motioncon_srv.tconf.priority = TASK_PRIORITY_MID;
  motioncon_srv.tconf.delay_ms = 200;
  motioncon_srv.tconf.period_ms = 20;

  motioncon_srv.dependencies.coordinator_interface = &(coord_srv.interface);

  motioncon_srv.sdata.desired_motion_msgq = &desired_motion_queue;

  ret = StartMotionService(&motioncon_srv);
  if (!ret) {
    printk("[ERROR] Failed to start motion control service\n");
    return false;
  } else {
    printk("[INFO] Started motion control service\n");
  }

  // messenger
  msger_srv.rx_tconf.priority = TASK_PRIORITY_HIGH;
  msger_srv.rx_tconf.delay_ms = 100;
  msger_srv.rx_tconf.period_ms = 20;

  msger_srv.tx_tconf.priority = TASK_PRIORITY_MID;
  msger_srv.tx_tconf.delay_ms = 100;
  msger_srv.tx_tconf.period_ms = 20;

  //   msger_srv.sdata.supervisor_cmd_msgq = &supervisor_cmd_queue;
  //   msger_srv.sdata.desired_motion_msgq = &can_desired_motion_queue;
  //   msger_srv.sdata.robot_state_msgq = &robot_state_queue;

  msger_srv.sconf.dd_can = GetCanDescriptor(RCCAR_CAN_UPLINK);
  //   msger_srv.dependencies.receiver_interface = &(rcvr_srv.interface);
  //   msger_srv.dependencies.actuator_interface = &(actr_srv.interface);
  //   msger_srv.dependencies.speed_control_interface =
  //   &(spdcon_srv.interface);

  //  msger_srv.sdata.encoder_rpm_msgq = &encoder_rpm_queue;

  ret = StartMessengerService(&msger_srv);
  if (!ret) {
    printk("[ERROR] Failed to start messenger service\n");
    return false;
  } else {
    printk("[INFO] Started messenger service\n");
  }

//   // coordinator
//   coord_srv.tconf.priority = TASK_PRIORITY_MID;
//   coord_srv.tconf.delay_ms = 0;
//   coord_srv.tconf.period_ms = 20;

//   coord_srv.sconf.dd_led_status = GetLedDescriptor(RCCAR_LED_STATUS);

//   //   coord_srv.sdata.robot_state_msgq = &robot_state_queue;

//   coord_srv.dependencies.receiver_interface = &(rcvr_srv.interface);
//   coord_srv.dependencies.motion_control_interface = &(motioncon_srv.interface);
//   coord_srv.dependencies.messenger_interface = &(msger_srv.interface);

//   ret = StartCoordinatorService(&coord_srv);
//   if (!ret) {
//     printk("[ERROR] Failed to start coordinator service\n");
//     return false;
//   } else {
//     printk("[INFO] Started coordinator service\n");
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

    k_msleep(200);
  }
}
