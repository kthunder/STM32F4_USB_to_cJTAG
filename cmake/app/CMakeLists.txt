cmake_minimum_required(VERSION 3.22)

project(app)
add_library(app INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(app INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(app INTERFACE
    ../../Core/RTT
)

target_sources(app INTERFACE
    ../../Core/RTT/log.c
    ../../Core/RTT/openocd_tool.c
    ../../Core/RTT/SEGGER_RTT.c
    ../../Core/RTT/SEGGER_RTT_ASM_ARMv7M.s
    ../../Core/Src/cJTAG.c
    # ../../Core/Src/W25Q32.c
)

target_link_directories(app INTERFACE
)

target_link_libraries(app INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


