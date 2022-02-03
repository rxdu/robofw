/*
 * tbot_interface.c
 *
 * Created on: Jan 27, 2022 21:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include "tbot_interface.h"

#include "mcal/hwconfig.h"

#include "receiver/receiver_service.h"
#include "actuator/actuator_service.h"
#include "light/light_service.h"

#include "coordinator/coordinator_service.h"
#include "motion/motion_service.h"

#define TBOT_LED_STATUS DD_LED0
#define TBOT_LED_USER1 DD_LED1
#define TBOT_LED_USER2 DD_LED2

// left motor
#define TBOT_DIO_EN1 DD_DIO0
#define TBOT_DIO_DIR1 DD_DIO1
#define TBOT_PWM1 DD_PWM0

// right motor
#define TBOT_DIO_EN2 DD_DIO2
#define TBOT_DIO_DIR2 DD_DIO3
#define TBOT_PWM2 DD_PWM1

// front light
#define TBOT_DIO_LIGHT_CTRL DD_DIO4

// rc input
#define TBOT_UART_SBUS DD_UART0

// gps receiver
#define TBOT_UART_GPS DD_UART1

// ultrasonic sensor
#define TBOT_UART_ULTRASONIC DD_UART2

// CAN uplink/downlink
#define TBOT_CAN_UPLINK DD_CAN0
#define TBOT_CAN_DOWNLINK DD_CAN1

typedef struct {
  LedDescription* leds;
  DioDescription* dios;
  PwmDescription* pwms;
  UartDescription* uarts;
  CanDescription* cans;
} RobotHardware;

typedef struct {
  ReceiverServiceConf rcvr_srv;
  ActuatorServiceConf actr_srv;
  LightServiceConf light_srv;
  CoordinatorServiceConf coord_srv;
  MotionServiceConf motion_srv;
} RobotService;

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
K_THREAD_STACK_DEFINE(receiver_service_stack, 512);

static TbotActuatorConf tbot_motor_cfg;
struct k_thread actuator_thread;
K_THREAD_STACK_DEFINE(actuator_service_stack, 512);

static LightConf light_cfg;
struct k_thread light_thread;
K_THREAD_STACK_DEFINE(light_service_stack, 512);

static LedConf led_cfg;
struct k_thread system_thread;
K_THREAD_STACK_DEFINE(system_service_stack, 512);

struct k_thread coord_thread;
K_THREAD_STACK_DEFINE(coord_service_stack, 512);

struct k_thread motion_thread;
K_THREAD_STACK_DEFINE(motion_service_stack, 1024);

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
  srv.actr_srv.priority = TASK_PRIORITY_HIGHEST;
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
    return false;
  } else {
    printk("[INFO] Started actuator service\n");
  }

  // receiver service
  srv.rcvr_srv.priority = TASK_PRIORITY_HIGHEST;
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
    return false;
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
    return false;
  } else {
    printk("[INFO] Started light service\n");
  }

  // coordinator
  srv.coord_srv.priority = TASK_PRIORITY_HIGH;
  srv.coord_srv.thread = &coord_thread;
  srv.coord_srv.stack = coord_service_stack;
  srv.coord_srv.stack_size = K_THREAD_STACK_SIZEOF(coord_service_stack);
  srv.coord_srv.delay = Z_TIMEOUT_MS(20);
  srv.coord_srv.period_ms = 20;

  led_cfg.dd_led_status = GetLedDescriptor(TBOT_LED_STATUS);
  srv.coord_srv.led_cfg = &led_cfg;
  srv.coord_srv.rcvr_srv = &srv.rcvr_srv;

  ret = StartCoordinatorService(&srv.coord_srv);
  if (!ret) {
    printk("[ERROR] Failed to start coordinator service\n");
    return false;
  } else {
    printk("[INFO] Started coordinator service\n");
  }

  // motion control
  srv.motion_srv.priority = TASK_PRIORITY_HIGH;
  srv.motion_srv.thread = &motion_thread;
  srv.motion_srv.stack = motion_service_stack;
  srv.motion_srv.stack_size = K_THREAD_STACK_SIZEOF(motion_service_stack);
  srv.motion_srv.delay = Z_TIMEOUT_MS(40);
  srv.motion_srv.period_ms = 20;

  srv.motion_srv.coord_srv = &srv.coord_srv;
  srv.motion_srv.actr_srv = &srv.actr_srv;

  ret = StartMotionService(&srv.motion_srv);
  if (!ret) {
    printk("[ERROR] Failed to start motion service\n");
    return false;
  } else {
    printk("[INFO] Started motion service\n");
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

void ShowRobotPanic() {
  LedDescriptor* led0 = GetLedDescriptor(TBOT_LED_STATUS);
  LedDescriptor* led1 = GetLedDescriptor(TBOT_LED_USER1);
  LedDescriptor* led2 = GetLedDescriptor(TBOT_LED_USER2);

  k_thread_abort(srv.rcvr_srv.tid);
  k_thread_abort(srv.actr_srv.tid);
  k_thread_abort(srv.light_srv.tid);
  k_thread_abort(srv.coord_srv.tid);
  //   k_thread_abort(srv.motion_srv.tid);

  TurnOnLed(led0);
  TurnOnLed(led1);
  TurnOnLed(led2);

  while (1) {
    ToggleLed(led0);
    ToggleLed(led1);
    ToggleLed(led2);

    k_msleep(1000);
  }
}