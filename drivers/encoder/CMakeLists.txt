# SPDX-License-Identifier: Apache-2.0

# zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ENCODER_STM32		encoder_stm32.c)

# zephyr_library_sources_ifdef(CONFIG_USERSPACE   pwm_handlers.c)
# zephyr_library_sources_ifdef(CONFIG_PWM_CAPTURE pwm_capture.c)
# zephyr_library_sources_ifdef(CONFIG_PWM_SHELL   pwm_shell.c)
