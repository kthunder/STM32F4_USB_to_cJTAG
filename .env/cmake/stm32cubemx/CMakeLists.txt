cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	STM32F401xC
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../src/app/Inc
    ../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ../../src/Drivers/CMSIS/Include
    ../../src/Drivers/HAL_Driver/Inc
    ../../src/Drivers/MinRTOS/Inc
    ../../src/threadx/common/inc
    ../../src/threadx/ports/inc
)

# add source files
file(GLOB_RECURSE SOURCES "../../src/*.[c|s|S]" "src/plugins/*.*")
foreach(var ${SOURCES})
    message("SOURCES = ${var}")
endforeach()
# exclued source files
file(GLOB_RECURSE EXCLUDE_SOURCES)
list(REMOVE_ITEM SOURCES ${EXCLUDE_SOURCES})

target_sources(stm32cubemx INTERFACE
    ${SOURCES}
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


