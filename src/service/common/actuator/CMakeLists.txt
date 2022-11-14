# add library
message(STATUS "[SERVICE] Enabled actuator service")

add_library(actuator 
    src/actuator_service.c
    src/tbot_actuators.c)
target_link_libraries(actuator PUBLIC zephyr mcal interface ${GCC_FLAG})
set_target_properties(actuator PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(actuator PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)