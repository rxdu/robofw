# add library
if (ROBOT STREQUAL "tbot")
    set(ROBOT_SRC src/tbot_interface.c)
endif()

add_library(robot ${ROBOT_SRC})
target_link_libraries(robot PUBLIC zephyr hwconfig ${GCC_FLAG})
set_target_properties(robot PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(robot PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)