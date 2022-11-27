# common services
add_subdirectory(${SERVICE_PATH}/common/receiver)
add_subdirectory(${SERVICE_PATH}/common/actuator)
add_subdirectory(${SERVICE_PATH}/common/coordinator)

# rccar-specific services
add_subdirectory(${SERVICE_PATH}/robot_rccar/messenger)
add_subdirectory(${SERVICE_PATH}/robot_rccar/motion_control)

set(SRV_MODULES
    receiver 
    actuator 
    messenger 
    motion_control 
    coordinator
)
