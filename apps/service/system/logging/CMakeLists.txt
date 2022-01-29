# add library
add_library(logging src/logging_service.c)
target_link_libraries(logging PUBLIC zephyr mcal ${GCC_FLAG})
set_target_properties(logging PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(logging PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)