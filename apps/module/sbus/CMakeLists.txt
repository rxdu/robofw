## Add libraries
add_library(sbus STATIC src/sbus.c)
target_link_libraries(sbus PUBLIC mcal ${GCC_FLAG})
set_target_properties(sbus PROPERTIES COMPILE_FLAGS ${ARCH_FLAGS})
target_include_directories(sbus PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src)