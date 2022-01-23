/* 
 * led_init.h
 * 
 * Created on: Jan 22, 2022 23:36
 * Description: 
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */ 

#ifndef LED_INIT_H
#define LED_INIT_H

#include <drivers/gpio.h>

typedef enum {
  DD_LED0 = 0,
  DD_LED1,
  DD_LED2,
  DD_LED3,
  DD_LED_NUM
} LedList;

// LED
typedef struct {
  bool active;
  const struct device *device;
  gpio_pin_t pin;
} LedDescriptor;

typedef struct {
  LedDescriptor descriptor[DD_LED_NUM];
} LedDescription;

bool InitLed();
LedDescription* GetLedDescription();
void PrintLedInitResult();

#endif /* LED_INIT_H */
