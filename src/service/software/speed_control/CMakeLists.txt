# add library
message(STATUS "[SERVICE] Enabled speed_control service")

add_library(speed_control src/speed_control_service.c)
target_link_libraries(speed_control PUBLIC zephyr interface receiver actuator ${GCC_FLAG})
set_target_properties(speed_control PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(speed_control PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)