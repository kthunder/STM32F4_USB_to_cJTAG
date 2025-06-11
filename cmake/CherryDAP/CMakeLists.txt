cmake_minimum_required(VERSION 3.22)

project(CherryDAP)
add_library(CherryDAP INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(CherryDAP INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(CherryDAP INTERFACE
    ../../libs/CherryDAP
    ../../libs/CherryDAP/DAP/Include
)

target_sources(CherryDAP INTERFACE
    ../../libs/CherryDAP/dap_main.c
    ../../libs/CherryDAP/DAP/Source/DAP_vendor.c
    ../../libs/CherryDAP/DAP/Source/DAP.c
    ../../libs/CherryDAP/DAP/Source/JTAG_DP.c
    ../../libs/CherryDAP/DAP/Source/SW_DP.c
    ../../libs/CherryDAP/DAP/Source/SWO.c
    ../../libs/CherryDAP/DAP/Source/UART.c
)

target_link_directories(CherryDAP INTERFACE
)

target_link_libraries(CherryDAP INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


