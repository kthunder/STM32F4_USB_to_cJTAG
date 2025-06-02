cmake_minimum_required(VERSION 3.22)

project(CherryUSB)
add_library(CherryUSB INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(CherryUSB INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(CherryUSB INTERFACE
    ../../libs/CherryUSB/core
    ../../libs/CherryUSB/common
    ../../libs/CherryUSB/class/hub
    ../../libs/CherryUSB/class/cdc
    ../../libs/CherryUSB/class/msc
    ../../libs/CherryUSB/demo
    ../../libs/CherryUSB/port/dwc2
)

target_sources(CherryUSB INTERFACE
    ../../libs/CherryUSB/core/usbd_core.c
    ../../libs/CherryUSB/port/dwc2/usb_dc_dwc2.c
    ../../libs/CherryUSB/port/dwc2/usb_glue_st.c
    ../../libs/CherryUSB/class/msc/usbd_msc.c
    ../../libs/CherryUSB/class/msc/usbd_msc.c
    ../../libs/CherryUSB/class/cdc/usbd_cdc_acm.c
)

target_link_directories(CherryUSB INTERFACE
)

target_link_libraries(CherryUSB INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


