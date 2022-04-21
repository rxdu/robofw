# add library
message(STATUS "[SERVICE] Enabled motion service")

add_library(motion src/motion_service.c)
target_link_libraries(motion PUBLIC zephyr interface receiver actuator ${GCC_FLAG})
set_target_properties(motion PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(motion PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)