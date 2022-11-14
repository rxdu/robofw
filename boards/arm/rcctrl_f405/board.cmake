# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=STM32F405RG" "--speed=4000")
board_runner_args(canopen "--node-id=${CONFIG_CANOPEN_NODE_ID}")

#include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)     ##comment when using jlink
include(${ZEPHYR_BASE}/boards/common/canopen.board.cmake)

set(BOARD_CPU "cm4")