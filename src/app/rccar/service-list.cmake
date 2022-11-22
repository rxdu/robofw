# common services
add_subdirectory(${SERVICE_PATH}/common/receiver)
add_subdirectory(${SERVICE_PATH}/common/actuator)
# add_subdirectory(${SERVICE_PATH}/common/coordinator)
# add_subdirectory(${SERVICE_PATH}/hardware/encoder)
# add_subdirectory(${SERVICE_PATH}/hardware/messenger)

# rccar-specific services
add_subdirectory(${SERVICE_PATH}/robot_rccar/messenger)
# add_subdirectory(${SERVICE_PATH}/software/coordinator)
# add_subdirectory(${SERVICE_PATH}/software/speed_control)
# add_subdirectory(${SERVICE_PATH}/software/motion_control)
