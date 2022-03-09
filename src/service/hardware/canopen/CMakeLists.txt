# add library
add_library(canopen src/canopen_service.c)
target_link_libraries(canopen PUBLIC zephyr mcal interface ${GCC_FLAG})
set_target_properties(canopen PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(canopen PUBLIC
        include
  ${CMAKE_CURRENT_SOURCE_DIR}/objdict)
