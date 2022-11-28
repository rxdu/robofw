# add library
message(STATUS "[SERVICE] Enabled coordinator service")

add_library(coordinator src/coordinator_service.c)
target_link_libraries(coordinator PUBLIC zephyr interface receiver messenger ${GCC_FLAG})
set_target_properties(coordinator PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(coordinator PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)