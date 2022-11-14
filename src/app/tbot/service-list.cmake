# hardware services
add_subdirectory(${SERVICE_PATH}/common/receiver)
add_subdirectory(${SERVICE_PATH}/common/actuator)
add_subdirectory(${SERVICE_PATH}/common/encoder)

# software services
add_subdirectory(${SERVICE_PATH}/robot_tbot/messenger)
add_subdirectory(${SERVICE_PATH}/robot_tbot/coordinator)
add_subdirectory(${SERVICE_PATH}/robot_tbot/speed_control)
add_subdirectory(${SERVICE_PATH}/robot_tbot/motion_control)
