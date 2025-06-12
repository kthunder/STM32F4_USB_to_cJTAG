cmake_minimum_required(VERSION 3.22)

project(CherryUSB)
add_library(CherryUSB INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(CherryUSB INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(CherryUSB INTERFACE
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/core
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/common
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/hub
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/cdc
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/msc
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/demo
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/port/dwc2

    ${CMAKE_SOURCE_DIR}/libs/CherryRB
)

target_sources(CherryUSB INTERFACE
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/core/usbd_core.c
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/port/dwc2/usb_dc_dwc2.c
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/port/dwc2/usb_glue_st.c
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/msc/usbd_msc.c
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/msc/usbd_msc.c
    ${CMAKE_SOURCE_DIR}/libs/CherryUSB/class/cdc/usbd_cdc_acm.c

    ${CMAKE_SOURCE_DIR}/libs/CherryRB/chry_ringbuffer.c
)

target_link_directories(CherryUSB INTERFACE
)

target_link_libraries(CherryUSB INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


