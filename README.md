# Robot Firmware

This repository contains firmware code based on [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr) for robot low-level control. 

## Target setup

* MCU: STM32F405RG/STM32F427VI
* Zephyr: v2.7 

## Setup workspace

### Install dependencies, west, toolchain, setup Zephyr

Follow the official instructions

https://docs.zephyrproject.org/2.5.0/getting_started/index.html

You may need to add the following environment variables to your ~/.bashrc. Remember to adjust the two paths according to your setups accordingly. For example, if you put zephyr at "~/zephyrproject/zephyr", then the variable "ZEPHYR_BASE" should equal to "~/zephyrproject/zephyr" instead of "~/RduWs/zephyrproject/zephyr".

```
export ZEPHYR_BASE=~/RduWs/zephyrproject/zephyr
export ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk-0.12.3
```

If everything is setup correctly, you should be able to compile a sample project

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
$ git checkout can_native_posix_v2.4
```

Update Zephyr dependencies

```
$ cd <path-of-zephyrproject>
$ west update
$ west zephyr-export
```

### Build firmware for the sensor module

```
$ cd <path-of-this-repo>/sensor_fw
$ west build
```

### Build and flash bootloader 

```
$ west build -d build-mcuboot -b bbb_racer_cape -s ~/RduWs/zephyrproject/bootloader/mcuboot/boot/zephyr/ -- -DBOARD_ROOT=/home/rdu/RduWs/auto_racing/firmware/zephyr
$ west flash -d build-mcuboot
```