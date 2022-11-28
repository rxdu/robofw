# common services
add_subdirectory(${SERVICE_PATH}/common/receiver)
add_subdirectory(${SERVICE_PATH}/common/actuator)

# rccar-specific services
add_subdirectory(${SERVICE_PATH}/robot_rccar/messenger)
add_subdirectory(${SERVICE_PATH}/robot_rccar/coordinator)

set(SRV_MODULES
    receiver 
    actuator 
    messenger 
    coordinator
)
