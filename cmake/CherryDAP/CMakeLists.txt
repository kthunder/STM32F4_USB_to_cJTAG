cmake_minimum_required(VERSION 3.22)

project(CherryDAP)
add_library(CherryDAP INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(CherryDAP INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

# set(DAP_PATH libs/CherryDAP/DAP)
set(DAP_PATH Drivers/CMSIS/DAP/Firmware)

target_include_directories(CherryDAP INTERFACE
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Include
)

target_sources(CherryDAP INTERFACE
    ${CMAKE_SOURCE_DIR}/Core/Src/dap_main.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/DAP_vendor.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/DAP.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/JTAG_DP.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/SW_DP.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/SWO.c
    ${CMAKE_SOURCE_DIR}/${DAP_PATH}/Source/UART.c
)

target_link_directories(CherryDAP INTERFACE
)

target_link_libraries(CherryDAP INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


