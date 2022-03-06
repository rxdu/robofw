# add library
add_library(interface INTERFACE)
target_link_libraries(interface INTERFACE zephyr mcal ${GCC_FLAG})
set_target_properties(interface PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(interface INTERFACE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
