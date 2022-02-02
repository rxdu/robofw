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

static RobotHardware hw;
static RobotService srv;

static SbusConf sbus_cfg;
struct k_thread receiver_thread;
K_THREAD_STACK_DEFINE(receiver_service_stack, 1024);

static TbotActuatorConf tbot_motor_cfg;
struct k_thread actuator_thread;
K_THREAD_STACK_DEFINE(actuator_service_stack, 512);

static LightConf light_cfg;
struct k_thread light_thread;
K_THREAD_STACK_DEFINE(light_service_stack, 512);

bool InitRobot() {
  // load all drivers from device tree
  if (!InitHardware()) return false;

  hw.leds = GetLedDescription();
  hw.dios = GetDioDescription();
  hw.pwms = GetPwmDescription();
  hw.uarts = GetUartDescription();
  hw.cans = GetCanDescription();

  bool ret = false;
  (void)ret;

  // configure drivers required by robot
  // LED for debugging
  TurnOffLed(&hw.leds->descriptor[TBOT_LED_STATUS]);
  TurnOffLed(&hw.leds->descriptor[TBOT_LED_USER1]);
  TurnOffLed(&hw.leds->descriptor[TBOT_LED_USER2]);

  // actuator service
  srv.actr_srv.priority = TASK_PRIORITY_HIGH;
  srv.actr_srv.thread = &actuator_thread;
  srv.actr_srv.stack = actuator_service_stack;
  srv.actr_srv.stack_size = K_THREAD_STACK_SIZEOF(actuator_service_stack);
  srv.actr_srv.delay = K_NO_WAIT;
  srv.actr_srv.period_ms = 20;

  srv.actr_srv.type = ACTR_TBOT;
  srv.actr_srv.active_motor_num = 2;
  tbot_motor_cfg.dd_dio_en1 = GetDioDescriptor(TBOT_DIO_EN1);
  tbot_motor_cfg.dd_dio_dir1 = GetDioDescriptor(TBOT_DIO_DIR1);
  tbot_motor_cfg.dd_dio_en2 = GetDioDescriptor(TBOT_DIO_EN2);
  tbot_motor_cfg.dd_dio_dir2 = GetDioDescriptor(TBOT_DIO_DIR2);
  tbot_motor_cfg.dd_pwm1 = GetPwmDescriptor(TBOT_PWM1);
  tbot_motor_cfg.dd_pwm2 = GetPwmDescriptor(TBOT_PWM2);
  srv.actr_srv.actuator_cfg = &tbot_motor_cfg;

  ret = StartActuatorService(&srv.actr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start actuator service\n");
  } else {
    printk("[INFO] Started actuator service\n");
  }

  // receiver service
  srv.rcvr_srv.priority = TASK_PRIORITY_HIGH;
  srv.rcvr_srv.thread = &receiver_thread;
  srv.rcvr_srv.stack = receiver_service_stack;
  srv.rcvr_srv.stack_size = K_THREAD_STACK_SIZEOF(receiver_service_stack);
  srv.rcvr_srv.delay = K_NO_WAIT;
  srv.rcvr_srv.period_ms = 0;

  srv.rcvr_srv.type = RCVR_SBUS;
  sbus_cfg.dd_uart = GetUartDescriptor(TBOT_UART_SBUS);
  srv.rcvr_srv.rcvr_cfg = &sbus_cfg;

  ret = StartReceiverService(&srv.rcvr_srv);
  if (!ret) {
    printk("[ERROR] Failed to start receiver service\n");
  } else {
    printk("[INFO] Started receiver service\n");
  }

  // light control
  srv.light_srv.priority = TASK_PRIORITY_MID;
  srv.light_srv.thread = &light_thread;
  srv.light_srv.stack = light_service_stack;
  srv.light_srv.stack_size = K_THREAD_STACK_SIZEOF(light_service_stack);
  srv.light_srv.delay = K_NO_WAIT;
  srv.light_srv.period_ms = 50;

  light_cfg.dd_dio_front = GetDioDescriptor(TBOT_DIO_LIGHT_CTRL);
  srv.light_srv.light_cfg = &light_cfg;

  ret = StartLightService(&srv.light_srv);
  if (!ret) {
    printk("[ERROR] Failed to start light service\n");
  } else {
    printk("[INFO] Started light service\n");
  }

  //   // gps receiver
  //   struct uart_config uart_test_cfg;
  //   uart_test_cfg.baudrate = 115200;
  //   uart_test_cfg.parity = UART_CFG_PARITY_NONE;
  //   uart_test_cfg.stop_bits = UART_CFG_STOP_BITS_1;
  //   uart_test_cfg.data_bits = UART_CFG_DATA_BITS_8;
  //   uart_test_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

  //   ConfigureUart(&hw.uarts->descriptor[TBOT_UART_GPS],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&hw.uarts->descriptor[TBOT_UART_GPS]);
  //   StartUartAsyncReceive(&hw.uarts->descriptor[TBOT_UART_GPS]);

  //   // ultrasonic sensor
  //   ConfigureUart(&hw.uarts->descriptor[TBOT_UART_ULTRASONIC],
  //                 uart_test_cfg);
  //   SetupUartAsyncMode(&hw.uarts->descriptor[TBOT_UART_ULTRASONIC]);
  //   StartUartAsyncReceive(
  //       &hw.uarts->descriptor[TBOT_UART_ULTRASONIC]);

  //   // uplink CAN to onboard computer
  //   struct zcan_filter can_filter;
  //   can_filter.id_type = CAN_STANDARD_IDENTIFIER;
  //   can_filter.rtr = CAN_DATAFRAME;
  //   can_filter.rtr_mask = 1;
  //   can_filter.id_mask = 0;
  //   ConfigureCan(&hw.cans->descriptor[TBOT_CAN_UPLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);
  //   ConfigureCan(&hw.cans->descriptor[TBOT_CAN_DOWNLINK],
  //                CAN_NORMAL_MODE, 500000, can_filter);

  printk("-----------------------------------------------------\n");

  return true;
}

RobotHardware* GetHardware() { return &hw; }

RobotService* GetService() { return &srv; }

void TurnOnLight() { SetDio(&hw.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 1); }

void TurnOffLight() { SetDio(&hw.dios->descriptor[TBOT_DIO_LIGHT_CTRL], 0); }
