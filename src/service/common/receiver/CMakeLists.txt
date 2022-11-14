# add library
message(STATUS "[SERVICE] Enabled receiver service")

add_library(receiver 
    src/receiver_service.c
    src/sbus_receiver.c)
target_link_libraries(receiver PUBLIC zephyr mcal interface sbus ${GCC_FLAG})
set_target_properties(receiver PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(receiver PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)