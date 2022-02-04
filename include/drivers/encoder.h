/*
 * Copyright (c) 2022 Ruixiang Du (ruixiang.du@gmail.co
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public ENCODER Driver APIs
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_ENCODER_H_
#define ZEPHYR_INCLUDE_DRIVERS_ENCODER_H_

/**
 * @brief ENCODER Interface
 * @defgroup encoder_interface ENCODER Interface
 * @ingroup io_interfaces
 * @{
 */

#include <errno.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <sys/math_extras.h>
#include <device.h>
// #include <dt-bindings/encoder/encoder.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides a type to hold ENCODER configuration flags.
 */
typedef uint8_t encoder_flags_t;

/**
 * @typedef encoder_pin_set_t
 * @brief Callback API upon setting the pin
 * See @a encoder_pin_set_cycles() for argument description
 */
typedef int (*encoder_pin_set_t)(const struct device *dev, uint32_t encoder,
			     uint32_t period_cycles, uint32_t pulse_cycles,
			     encoder_flags_t flags);

/**
 * @typedef encoder_get_counts_t
 * @brief Callback API upon getting cycles per second
 * See @a encoder_get_cycles_per_sec() for argument description
 */
typedef int (*encoder_get_counts_t)(const struct device *dev,
					uint32_t encoder,
					uint64_t *cycles);

/** @brief ENCODER driver API definition. */
__subsystem struct encoder_driver_api {
	encoder_pin_set_t pin_set;
	encoder_get_counts_t get_counts;
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

// #include <syscalls/encoder.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_ENCODER_H_ */
