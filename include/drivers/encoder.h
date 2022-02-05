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
#include <dt-bindings/encoder/encoder.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides a type to hold ENCODER configuration flags.
 */
typedef uint8_t encoder_flags_t;

/**
 * @typedef encoder_get_counts_t
 * @brief Get the counter value
 * See @a encoder_get_cycles_per_sec() for argument description
 */
typedef int (*encoder_get_counts_t)(const struct device *dev, uint32_t *counts);

/**
 * @typedef encoder_is_counting_up_t
 * @brief Callback API upon getting counts
 * See @a encoder_get_cycles_per_sec() for argument description
 */
typedef int (*encoder_is_counting_up_t)(const struct device *dev,
                                        bool *counting_up);

/** @brief ENCODER driver API definition. */
struct encoder_driver_api {
  encoder_get_counts_t get_counts;
  encoder_is_counting_up_t is_counting_up;
};

static inline int encoder_get_counts(const struct device *dev,
                                     uint32_t *counts) {
  const struct encoder_driver_api *api = (struct encoder_driver_api *)dev->api;

  if (api->get_counts == NULL) {
    return -ENOSYS;
  }

  return api->get_counts(dev, counts);
}

static inline int encoder_is_counting_up(const struct device *dev,
                                         bool *counting_up) {
  const struct encoder_driver_api *api = (struct encoder_driver_api *)dev->api;

  if (api->is_counting_up == NULL) {
    return -ENOSYS;
  }

  return api->is_counting_up(dev, counting_up);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

// #include <syscalls/encoder.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_ENCODER_H_ */
