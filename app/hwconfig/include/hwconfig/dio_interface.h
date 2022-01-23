/*
 * dio_interface.h
 *
 * Created on: Jan 23, 2022 12:55
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef DIO_INTERFACE_H
#define DIO_INTERFACE_H

#include "hwconfig/dio_init.h"

void ConfigureDio(DioDescriptor* dd, gpio_flags_t flags);
void SetDio(DioDescriptor* dd, uint8_t value);
void ToggleDio(DioDescriptor* dd);

#endif /* DIO_INTERFACE_H */
