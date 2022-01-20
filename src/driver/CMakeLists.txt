# add library
if (BUILD_POSIX_NATIVE)
set(DRIVER_SRC 
    src/native_posix/led.c
    src/native_posix/can.c)
else()
  set(DRIVER_SRC 
    src/stm32/gpio.c
    src/stm32/led.c
    # src/stm32/can.c
    src/stm32/pwm.c)
endif()

add_library(driver ${DRIVER_SRC})
target_link_libraries(driver PUBLIC zephyr ${GCC_FLAG})
set_target_properties(driver PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(driver PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)