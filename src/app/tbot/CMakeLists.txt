zephyr_include_directories(objdict)

target_sources(app PRIVATE main.c tbot_interface.c objdict/CO_OD.c)
target_link_libraries(app PRIVATE zephyr 
    receiver actuator )
    # light speed_control
    # coordinator motion canopen)
