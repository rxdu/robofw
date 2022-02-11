/*
 * Copyright (c) 2022 Ruixiang Du (ruixiang.du@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_ENCODER_ENCODER_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_ENCODER_ENCODER_H_

/**
 * @name Encoder mode
 * The `ENCODER_MODE_*` flags are used with pwm_pin_set_cycles(),
 * pwm_pin_set_usec(), pwm_pin_set_nsec() or pwm_pin_configure_capture() to
 * specify the polarity of a PWM pin.
 * @{
 */
/** Encoder mode: count from TI1 */
#define ENCODER_MODE_X2_TI1	(0 << 0)

/** Encoder mode: count from TI2 */
#define ENCODER_MODE_X2_TI2	(1 << 0)

/** Encoder mode: count from TI1 and TI2 */
#define ENCODER_MODE_X4_TI12	(2 << 0)

/** @cond INTERNAL_HIDDEN */
#define ENCODER_MODE_MASK	0x1
/** @endcond */
/** @} */

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_ENCODER_ENCODER_H_ */
