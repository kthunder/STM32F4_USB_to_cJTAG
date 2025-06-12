cmake_minimum_required(VERSION 3.22)

project(app)
add_library(app INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(app INTERFACE 
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(app INTERFACE
    # ${CMAKE_SOURCE_DIR}/Core/RTT
)

target_sources(app INTERFACE
    ${CMAKE_SOURCE_DIR}/Core/Src/log.c
    ${CMAKE_SOURCE_DIR}/Core/Src/cJTAG.c
    ${CMAKE_SOURCE_DIR}/Core/Src/winusb1.0_template.c
)

target_link_directories(app INTERFACE
)

target_link_libraries(app INTERFACE
)

# Validate that app code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


