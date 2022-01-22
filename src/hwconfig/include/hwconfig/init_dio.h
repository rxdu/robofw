/* 
 * init_dio.h
 * 
 * Created on: Jan 22, 2022 23:34
 * Description: 
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */ 

#ifndef INIT_DIO_H
#define INIT_DIO_H

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
  const struct device *dev;
  gpio_pin_t pin;
  gpio_flags_t flags;
} DioDescriptor;

typedef struct {
  DioDescriptor descriptor[DD_DIO_NUM];
} DioDescription;

bool InitDio();
DioDescription* GetDioDescription();
void PrintDioInitResult();

#endif /* INIT_DIO_H */
