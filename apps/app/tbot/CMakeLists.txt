target_sources(app PRIVATE main.c tbot_interface.c)
target_link_libraries(app PRIVATE zephyr receiver actuator light coordinator motion)