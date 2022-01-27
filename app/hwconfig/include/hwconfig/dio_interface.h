/*
 * dio_interface.h
 *
 * Created on: Jan 23, 2022 12:55
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef DIO_INTERFACE_H
#define DIO_INTERFACE_H

#include <drivers/gpio.h>

typedef enum {
  DD_DIO0 = 0,
  DD_DIO1,
  DD_DIO2,
  DD_DIO3,
  DD_DIO4,
  DD_DIO5,
  DD_DIO_NUM
} DioList;

// DIO
typedef struct {
  bool active;
  const struct device *device;
  gpio_pin_t pin;
  gpio_flags_t flags;
} DioDescriptor;

typedef struct {
  DioDescriptor descriptor[DD_DIO_NUM];
} DioDescription;

bool InitDio();
DioDescription* GetDioDescription();
void PrintDioInitResult();

void ConfigureDio(DioList dev_id, gpio_flags_t flags);
void SetDio(DioList dev_id, uint8_t value);
void ToggleDio(DioList dev_id);

#endif /* DIO_INTERFACE_H */
