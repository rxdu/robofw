# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE src/main.c objdict/CO_OD.c)
zephyr_include_directories(objdict)
