# add library
add_library(light 
    src/light_service.c)
target_link_libraries(light PUBLIC zephyr mcal sbus ${GCC_FLAG})
set_target_properties(light PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(light PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)