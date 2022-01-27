# Main application 
# target_sources(app PRIVATE racer/main.c)
# target_link_libraries(app PRIVATE bsp)

# target_sources(app PRIVATE native_posix/main.c)
# target_link_librariestarget_sources(app PRIVATE hello_world/main.c)
# target_sources(app PRIVATE blinky/main.c)

target_sources(app PRIVATE tbot/main.c)
target_link_libraries(app PRIVATE robot)
