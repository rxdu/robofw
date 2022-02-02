# add library
add_library(rc_control src/rc_control_service.c)
target_link_libraries(rc_control PUBLIC zephyr mcal ${GCC_FLAG})
set_target_properties(rc_control PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(rc_control PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)