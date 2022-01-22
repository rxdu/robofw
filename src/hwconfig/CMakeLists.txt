# add library
if (BUILD_POSIX_NATIVE)
    set(DRIVER_SRC 
        src/native_posix/led.c
        src/native_posix/can.c)
else()
    set(DRIVER_SRC 
        src/init_dio.c
        src/init_led.c)
endif()

add_library(hwconfig ${DRIVER_SRC})
target_link_libraries(hwconfig PUBLIC zephyr ${GCC_FLAG})
set_target_properties(hwconfig PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(hwconfig PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)