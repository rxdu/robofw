# add library
message(STATUS "[SERVICE] Enabled robotconf service")

add_library(robotconf INTERFACE)
target_include_directories(robotconf INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
