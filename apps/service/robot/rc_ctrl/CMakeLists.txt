# add library
add_library(sysinit src/rc_ctrl_service.c)
target_link_libraries(sysinit PUBLIC zephyr mcal ${GCC_FLAG})
set_target_properties(sysinit PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(sysinit PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)