# SPDX-License-Identifier: Apache-2.0

zephyr_library()

if (CONFIG_IAM20680)
  zephyr_library_sources(icm42605.c)
  zephyr_library_sources(icm42605_setup.c)
  zephyr_library_sources_ifdef(CONFIG_SPI icm42605_spi.c)
  zephyr_library_sources_ifdef(CONFIG_IAM20680_TRIGGER icm42605_trigger.c)
endif()
