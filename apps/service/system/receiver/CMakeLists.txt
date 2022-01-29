# add library
add_library(receiver_srv src/receiver_service.c)
target_link_libraries(receiver_srv PUBLIC zephyr mcal ${GCC_FLAG})
set_target_properties(receiver_srv PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(receiver_srv PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)