# hardware services
add_subdirectory(${SERVICE_PATH}/hardware/receiver)
add_subdirectory(${SERVICE_PATH}/hardware/actuator)
add_subdirectory(${SERVICE_PATH}/hardware/encoder)
add_subdirectory(${SERVICE_PATH}/hardware/messenger)

# software services
add_subdirectory(${SERVICE_PATH}/software/coordinator)
