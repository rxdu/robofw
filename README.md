# Robot Firmware

This repository contains firmware code based on [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr) for robot
low-level control.

## Target setup

* MCU: STM32F405RG/STM32F427VI
* Zephyr: v2.7

## Setup workspace

### Install dependencies, west, toolchain, setup Zephyr

Follow the official [getting started instructions](https://docs.zephyrproject.org/2.7.0/getting_started/index.html).

You may need to add the following environment variables to your \~/.bashrc. 

```
export ZEPHYR_BASE=~/RduWs/zephyrproject/zephyr
export ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk-0.12.3
```

Remember to adjust the two paths according to your setup accordingly. For example, if you put zephyr at "\~/zephyrproject/zephyr", then the variable "ZEPHYR_BASE" should equal to "\~/zephyrproject/zephyr" instead of "\~/RduWs/zephyrproject/zephyr".

If everything is set up correctly, you should be able to compile a sample project

```
$ cd <path-of-zephyrproject>/zephyr
$ west build -p auto -b native_posix samples/hello_world
```

### Checkout the custom development branch of Zephyr

```
$ cd <path-of-zephyrproject>/zephyr
$ git remote rename origin upstream
$ git remote add origin https://github.com/rxdu/zephyr.git
$ git remote fetch origin
$ git checkout rcu-zephyr-v2.7
```

Update Zephyr dependencies

```
$ cd <path-of-zephyrproject>
$ west update
$ west zephyr-export
```

### Build and flash bootloader

```
$ cd <path-of-this-repo>
$ make bl_rcctrl_f405
$ make flash_bootloader
```

### Build and flash firmware

```
$ cd <path-of-this-repo>
$ make fw_rcctrl_f405
$ make flash_rcctrl_f405
```

## Use JLink Tracing

* prj.conf

Comment out "CONFIG_UART_CONSOLE=y" and add the following configurations:

```
CONFIG_USE_SEGGER_RTT=y
CONFIG_RTT_CONSOLE=y
CONFIG_TRACING=y
CONFIG_SEGGER_SYSTEMVIEW=y
```

* board/*/board.cmake

Comment out the "openocd.board.cmake" and uncomment the "jlink.board.cmake" line:

```
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
#include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)  
```

## Reference

* [1] https://docs.zephyrproject.org/2.7.0/guides/debug_tools/tracing/index.html?highlight=systemview
