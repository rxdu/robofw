# add library
message(STATUS "[SERVICE] Enabled motion_control service")

add_library(motion_control src/motion_control_service.c)
target_link_libraries(motion_control PUBLIC zephyr interface receiver actuator ${GCC_FLAG})
set_target_properties(motion_control PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(motion_control PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)