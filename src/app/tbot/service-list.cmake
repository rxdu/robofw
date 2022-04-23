# hardware services
add_subdirectory(${SERVICE_PATH}/hardware/receiver)
add_subdirectory(${SERVICE_PATH}/hardware/actuator)
add_subdirectory(${SERVICE_PATH}/hardware/encoder)
add_subdirectory(${SERVICE_PATH}/hardware/messenger)

# software services
add_subdirectory(${SERVICE_PATH}/software/coordinator)
add_subdirectory(${SERVICE_PATH}/software/speed_control)
add_subdirectory(${SERVICE_PATH}/software/motion_control)
