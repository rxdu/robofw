## Add libraries
add_library(vesc STATIC 
    src/vesc_cmd_packet.c
    src/vesc_cmd_parser.c)
target_link_libraries(vesc PUBLIC mcal ${GCC_FLAG})
set_target_properties(vesc PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(vesc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src)