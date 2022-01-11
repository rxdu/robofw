# add library
if (BUILD_POSIX_NATIVE)
set(BSP_SRC 
    src/native_posix/led.c
    src/native_posix/can.c)
else()
  set(BSP_SRC 
    src/stm32/led.c
    src/stm32/can.c
    src/stm32/pwm.c)
endif()

add_library(bsp ${BSP_SRC})
target_link_libraries(bsp PUBLIC zephyr ${GCC_FLAG})
set_target_properties(bsp PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(bsp PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)