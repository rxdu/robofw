# add library
message(STATUS "[SERVICE] Enabled messenger service")

add_library(messenger src/messenger_service.c)
target_link_libraries(messenger PUBLIC zephyr mcal interface ${GCC_FLAG})
set_target_properties(messenger PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(messenger PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
