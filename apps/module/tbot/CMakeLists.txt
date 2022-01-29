# add library
if (ROBOT STREQUAL "tbot")
    set(ROBOT_SRC src/tbot_interface.c)
endif()

add_library(tbot ${ROBOT_SRC})
target_link_libraries(tbot PUBLIC zephyr mcal ${GCC_FLAG})
set_target_properties(tbot PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(tbot PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)