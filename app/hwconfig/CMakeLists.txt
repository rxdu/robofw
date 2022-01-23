# add library
if (BUILD_POSIX_NATIVE)
    set(DRIVER_SRC 
        src/native_posix/led.c
        src/native_posix/can.c)
else()
    set(DRIVER_SRC 
        src/dio_init.c
        src/led_init.c
        src/pwm_init.c
        src/dio_interface.c
        src/led_interface.c
        src/pwm_interface.c)
endif()

add_library(hwconfig ${DRIVER_SRC})
target_link_libraries(hwconfig PUBLIC zephyr ${GCC_FLAG})
set_target_properties(hwconfig PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(hwconfig PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)