cmake_minimum_required(VERSION 3.13.1)

## Project options
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_NATIVE_POSIX "Build as POSIX native target" OFF)

## Global setup
# Boards: bb_racer_cape, native_posix
if(BUILD_NATIVE_POSIX)
    set(BOARD "native_posix")
    add_definitions("-DTARGET_NATIVE_POSIX")
else()
    set(BOARD "tbotctrl_f405")
    add_definitions("-DTARGET_CORTEX_M4")
endif()

if(BUILD_TESTING)
set(BUILD_TESTS ON)
endif()

## Check whether ccache exists for faster compile
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

## Find project dependencies
# Setup Zephyr RTOS
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Zephyr 2.7.0)

## Project setup (must be called after zephyr setup)
project(robofw)

## Add source code
add_subdirectory(app)
