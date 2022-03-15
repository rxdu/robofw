zephyr_include_directories(objdict)

target_sources(app PRIVATE main.c tbot_interface.c)
target_link_libraries(app PRIVATE zephyr 
    receiver actuator coordinator)
    # speed_control motion )
