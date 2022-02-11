/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/sensor.h>

#include "bsp/interface.h"

#define SLEEP_TIME_MS 500

static const struct device* SetupIIS2MDC() {
  //   //   const struct device* dev = device_get_binding(LED0);
  //   //   ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
  //   //   if (ret < 0) {
  //   //     return;
  //   //   }

//   struct spi_cs_control* ctrl =
//       SPI_CS_CONTROL_PTR_DT(DT_NODELABEL(mag_iis2mdc), 2);
//   if (ctrl == NULL) printk("failed to get spi cs control\n");
//   gpio_port_clear_bits_raw(ctrl->gpio_dev, ctrl->gpio_pin);

  //   // const struct device* nss =
  //   //     device_get_binding(DT_LABEL(DT_INST(0, st_iis2mdc)));
  //   // gpio_pin_set(nss, led_cfg.leds[0].pin, 1);

  //   const struct device* dev =
  //       device_get_binding(DT_LABEL(DT_INST(0, st_iis2mdc)));
  const struct device* dev =
      device_get_binding(DT_LABEL(DT_NODELABEL(mag_iis2mdc)));

  if (dev == NULL) {
    /* No such node, or the node does not have status "okay". */
    // printk("[Error] no device found: %s.\n",
    //        DT_LABEL(DT_INST(0, st_iis2mdc)));
    printk("[Error] no device found: %s.\n",
           DT_LABEL(DT_NODELABEL(mag_iis2mdc)));
    return NULL;
  }

  //   //   if (device_is_ready(dev)) {
  //   //     printk("Device ready: %s.\n", DT_LABEL(DT_INST(0, st_iis2mdc)));
  //   //   }

  //   //   printk("Found device \"%s\", getting sensor data\n", dev->name);

  return dev;
}

#define PWM_LABEL DT_LABEL(DT_NODELABEL(pwm3))

void main(void) {
  printk("Starting board: %s\n", CONFIG_BOARD);

  if (!SetupLed()) printk("[ERROR]: Failed to setup LED\n");
  //   if (!CanSetup(UPLINK_CAN, CAN_NORMAL_MODE, 1000000))
  //     printk("[ERROR]: Failed to setup CAN\n");

  if (!SetupPwm()) printk("[ERROR]: Failed to setup PWM\n");

  SetPwmDutyCycle(SERVO_PWM, 0.05);
  SetPwmDutyCycle(MOTOR_PWM, 0.05);

  TurnOnLed(USER_LED0);
  TurnOnLed(USER_LED1);
  TurnOnLed(USER_LED2);
  TurnOnLed(USER_LED3);

  k_msleep(SLEEP_TIME_MS);

  const struct device* mag = SetupIIS2MDC();

  //   uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  struct sensor_value val;
  int rc;
  const struct device* stm32_temp_sensor =
      DEVICE_DT_GET(DT_INST(0, st_stm32_temp));

  if (!device_is_ready(stm32_temp_sensor)) {
    printk("Temperature sensor is not ready\n");
    return;
  }

  uint8_t count = 0;

  while (1) {
    if (count % 4 == 0)
      ToggleLed(USER_LED0);
    else if (count % 4 == 1)
      ToggleLed(USER_LED1);
    else if (count % 4 == 2)
      ToggleLed(USER_LED2);
    else if (count % 4 == 3)
      ToggleLed(USER_LED3);

    {
      rc = sensor_sample_fetch(stm32_temp_sensor);
      if (rc) {
        printk("Failed to fetch sample (%d)\n", rc);
        continue;
      }

      rc = sensor_channel_get(stm32_temp_sensor, SENSOR_CHAN_DIE_TEMP, &val);
      if (rc) {
        printk("Failed to get data (%d)\n", rc);
        continue;
      }

    //   printk("Current temperature: %.1f C\n", sensor_value_to_double(&val));
    }

    // {
    //   struct sensor_value val[3];
    //   int32_t ret = 0;

    //   ret = sensor_sample_fetch(mag);
    //   if (ret < 0 && ret != -EBADMSG) {
    //     printk("Sensor sample update error\n");
    //   } else {
    //     ret = sensor_channel_get(mag, SENSOR_CHAN_MAGN_XYZ, val);
    //     if (ret < 0) {
    //       printk("Cannot read sensor channels\n");
    //     } else {
    //       printk(
    //           "( x y z ) = ( %f  %f  %f )\n",
    //           sensor_value_to_double(&val[0]),
    //           sensor_value_to_double(&val[1]),
    //           sensor_value_to_double(&val[2]));
    //     }
    //   }
    // }

    // data[7]++;
    // SendCanFrame(UPLINK_CAN, 121, true, data, 8);

    ++count;
    k_msleep(SLEEP_TIME_MS);
  }
}
