# common services
add_subdirectory(${SERVICE_PATH}/common/receiver)
add_subdirectory(${SERVICE_PATH}/common/actuator)
add_subdirectory(${SERVICE_PATH}/common/encoder)

# tbot-specific services
add_subdirectory(${SERVICE_PATH}/robot_tbot/messenger)
add_subdirectory(${SERVICE_PATH}/robot_tbot/speed_control)
add_subdirectory(${SERVICE_PATH}/robot_tbot/motion_control)
add_subdirectory(${SERVICE_PATH}/robot_tbot/coordinator)

set(SRV_MODULES
    receiver 
    actuator 
    messenger 
    coordinator 
    encoder 
    speed_control 
    motion_control
)