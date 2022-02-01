/*
 * tbot_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "tbot_interface.h"

#include <zephyr.h>

// Negative prio threads will not be pre-empted
#define TASK_PRIORITY_VIP -1
#define TASK_PRIORITY_HIGHEST 1
#define TASK_PRIORITY_HIGH 2
#define TASK_PRIORITY_MID 3
#define TASK_PRIORITY_LOW 4

static RobotHardware robot_hardware;
static RobotService robot_service;

struct k_thread receiver_thread;
K_THREAD_STACK_DEFINE(receiver_service_stack, 1024);

struct k_thread actuator_thread;
K_THREAD_STACK_DEFINE(actuator_service_stack, 512);

bool InitRobot() {
  // load all drivers from device tree
  if (!InitHardware()) return false;

  robot_hardware.leds = GetLedDescription();
  robot_hardware.dios = GetDioDescription();
  robot_hardware.pwms = GetPwmDescription();
  robot_hardware.uarts = GetUartDescription();
  robot_hardware.cans = GetCanDescription();

  bool ret = false;
  (void)ret;

  // configure drivers required by robot
  // LED for debugging
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_STATUS]);
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_USER1]);
  TurnOffLed(&robot_hardware.leds->descriptor[TBOT_LED_USER2]);

  // motor control service
  //   robot_service.actr_srv.priority = TASK_PRIORITY_MID;
  //   robot_service.actr_srv.thread = &actuator_thread;
  //   robot_service.actr_srv.stack = actuator_service_stack;
  //   robot_service.actr_srv.stack_size =
  //       K_THREAD_STACK_SIZEOF(actuator_service_stack);
  //   robot_service.actr_srv.delay = K_NO_WAIT;

  //   robot_service.actr_srv.type = ACTR_TBOT;
  //   robot_service.actr_srv.active_motor_num = 2;
  //   TbotBrushedMotorConf tbot_motor_cfg;
  //   tbot_motor_cfg.dd_dio_en1 = GetDioDescriptor(TBOT_DIO_EN1);
  //   tbot_motor_cfg.dd_dio_dir1 = GetDioDescriptor(TBOT_DIO_DIR1);
  //   tbot_motor_cfg.dd_dio_en2 = GetDioDescriptor(TBOT_DIO_EN2);
  //   tbot_motor_cfg.dd_dio_dir2 = GetDioDescriptor(TBOT_DIO_DIR2);
  //   tbot_motor_cfg.dd_pwm1 = GetPwmDescriptor(TBOT_PWM1);
  //   tbot_motor_cfg.dd_pwm2 = GetPwmDescriptor(TBOT_PWM2);
  //   robot_service.actr_srv.actuator_cfg = &tbot_motor_cfg;

  //   ret = StartActuatorService(&robot_service.actr_srv);
  //   if (!ret) {
  //     printk("[ERROR] Failed to start actuator service\n");
  //   } else {
  //     printk("[INFO] Started actuator service\n");
  //   }

  // rc input service
  robot_service.rcvr_srv.priority = TASK_PRIORITY_HIGH;
  robot_service.rcvr_srv.thread = &receiver_thread;
  robot_service.rcvr_srv.stack = receiver_service_stack;
  robot_service.rcvr_srv.stack_size =
      K_THREAD_STACK_SIZEOF(receiver_service_stack);
  robot_service.rcvr_srv.delay = K_NO_WAIT;

  robot_service.rcvr_srv.type = RCVR_SBUS;
  SbusConf sbus_cfg;
  sbus_cfg.dd_uart = GetUartDescriptor(TBOT_UART_SBUS);
  robot_service.rcvr_srv.rcvr_cfg = &sbus_cfg;

  ret = StartReceiverService(&robot_service.rcvr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start receiver service\n");
  } else {
    printk("[INFO] Started receiver service\n");
  }

  // light control
  ConfigureDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL],
               GPIO_OUTPUT_ACTIVE | GPIO_PULL_UP);
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 0);

  //   // gps receiver
  //   struct uart_config uart_test_cfg;
  //   uart_test_cfg.baudrate = 115200;
  //   uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  //   uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  //   uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  //   uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  //   ConfigureUart(&robot_hardware.uarts->descriptor[TBOT_UART_GPS],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&robot_hardware.uarts->descriptor[TBOT_UART_GPS]);
  //   StartUartAsyncReceive(&robot_hardware.uarts->descriptor[TBOT_UART_GPS]);

  //   // ultrasonic sensor
  //   ConfigureUart(&robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC]);
  //   StartUartAsyncReceive(
  //       &robot_hardware.uarts->descriptor[TBOT_UART_ULTRASONIC]);

  //   // uplink CAN to onboard computer
  //   struct zcan_filter can_filter;
  //   can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  //   can_filter.rtr = CAN_DATAFRAME;
  //   can_filter.rtr_mask = 1;
  //   can_filter.id_mask = 0;
  //   ConfigureCan(&robot_hardware.cans->descriptor[TBOT_CAN_UPLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);
  //   ConfigureCan(&robot_hardware.cans->descriptor[TBOT_CAN_DOWNLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);

  printk("-----------------------------------------------------\n");

  return true;
}

RobotHardware* GetHardware() { return &robot_hardware; }

RobotService* GetService() { return &robot_service; }

void TurnOnLight() {
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 1);
}

void TurnOffLight() {
  SetDio(&robot_hardware.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 0);
}
