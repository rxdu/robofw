/*
 * led_interface.h
 *
 * Created on: Jan 23, 2022 12:57
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

#include <drivers/gpio.h>

typedef enum { DD_LED0 = 0, DD_LED1, DD_LED2, DD_LED3, DD_LED_NUM } LedList;

// LED
typedef struct {
  bool active;
  const struct device* device;
  gpio_pin_t pin;
} LedDescriptor;

typedef struct {
  LedDescriptor descriptor[DD_LED_NUM];
} LedDescription;

bool InitLed();
LedDescription* GetLedDescription();
LedDescriptor *GetLedDescriptor(LedList dev_id);
void PrintLedInitResult();

void TurnOnLed(LedDescriptor *dd);
void TurnOffLed(LedDescriptor *dd);
void ToggleLed(LedDescriptor *dd);

#endif /* LED_INTERFACE_H */
