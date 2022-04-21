cmake_minimum_required(VERSION 3.13.1)

## Project options
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_NATIVE_POSIX "Build as POSIX native target" OFF)

## Global setup
# Boards: bb_racer_cape, native_posix
if (BUILD_NATIVE_POSIX)
    set(BOARD "native_posix")
    add_definitions("-DTARGET_NATIVE_POSIX")
else ()
    add_definitions("-DTARGET_CORTEX_M4")
endif ()

# default app and board if not specified
if (NOT DEFINED APP_NAME)
    set(APP_NAME "tbot")
    #    set(APP_NAME "hwconfig")
endif ()
if (NOT DEFINED BOARD)
    set(BOARD "tbotctrl_f405")
endif ()

add_definitions("-DAPP_${APP_NAME}")

list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_TESTING)
    set(BUILD_TESTS ON)
endif ()

## Check whether ccache exists for faster compile
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    message(STATUS "Found ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif ()

## Find project dependencies
# Setup Zephyr RTOS
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Zephyr 2.7.0)

## Project setup (must be called after zephyr setup)
project(robofw)

# Architecture-specific flags
if (BUILD_POSIX_NATIVE)
    set(ARCH_FLAGS "-m32")
else ()
    set(CPU "-mcpu=cortex-m4")
    set(FPU "-mfpu=fpv4-sp-d16")
    set(FLOAT_ABI "-mfloat-abi=hard")
    set(ARCH_FLAGS "${CPU} -mthumb -mabi=aapcs ${FPU} ${FLOAT_ABI}")
endif ()

set(GCC_FLAG "-lgcc")

## Add source code
zephyr_include_directories(include)
zephyr_include_directories(drivers)

add_subdirectory(src)
add_subdirectory(drivers)

# Add services per application
set(APP_PATH ${CMAKE_CURRENT_LIST_DIR}/src/app/${APP_NAME})
set(SERVICE_PATH ${CMAKE_SOURCE_DIR}/src/service)
include(${APP_PATH}/service-list.cmake)