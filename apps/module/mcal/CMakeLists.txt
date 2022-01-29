# add library
if (BUILD_POSIX_NATIVE)
    set(DRIVER_SRC 
        src/native_posix/led.c
        src/native_posix/can.c)
else()
    set(DRIVER_SRC 
        src/hwconfig.c
        src/dio_interface.c
        src/led_interface.c
        src/pwm_interface.c
        src/uart_interface.c
        src/can_interface.c)
endif()

add_library(mcal ${DRIVER_SRC})
target_link_libraries(mcal PUBLIC zephyr ${GCC_FLAG})
set_target_properties(mcal PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
# target_compile_definitions(mcal PUBLIC HWCONFIG_ENABLE_ALL)
target_include_directories(mcal PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)