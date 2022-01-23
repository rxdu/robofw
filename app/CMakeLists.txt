# Architecture-specific flags
if (BUILD_POSIX_NATIVE)
    set(ARCH_FLAGS "-m32")
else()
    set(CPU "-mcpu=cortex-m4")
    set(FPU "-mfpu=fpv4-sp-d16")
    set(FLOAT_ABI "-mfloat-abi=hard")
    set(ARCH_FLAGS "${CPU} -mthumb -mabi=aapcs ${FPU} ${FLOAT_ABI}")
endif()

set(GCC_FLAG "-lgcc")

# Main application 
add_subdirectory(app)
add_subdirectory(driver)
add_subdirectory(hwconfig)
