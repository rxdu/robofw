# add library
message(STATUS "[SERVICE] Enabled encoder service")

add_library(encoder src/encoder_service.c)
target_link_libraries(encoder PUBLIC zephyr mcal interface actuator ${GCC_FLAG})
set_target_properties(encoder PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(encoder PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
