# add library
add_library(speed_control src/speed_control_service.c)
target_link_libraries(speed_control PUBLIC zephyr mcal actuator ${GCC_FLAG})
set_target_properties(speed_control PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(speed_control PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
