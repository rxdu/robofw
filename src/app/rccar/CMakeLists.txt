target_sources(app PRIVATE main.c rccar_interface.c)
target_link_libraries(app PRIVATE zephyr mcal
        receiver actuator)
