# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=STM32F427VI" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)     ##comment when using jlink
# include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)

set(BOARD_CPU "cm4")