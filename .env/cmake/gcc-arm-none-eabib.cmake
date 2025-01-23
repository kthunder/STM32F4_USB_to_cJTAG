set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_ASM_COMPILER              ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++.exe)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++.exe)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy.exe)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size.exe)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O0 -g3)
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_options(-Os -g0)
endif()

add_compile_options(
    -march=armv7e-m
    -mcpu=cortex-m4
    # -mfpu=fpv4-sp-d16
    # -mfloat-abi=hard
    -mthumb
    -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections
)

add_compile_options(
    $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>
)
# add_compile_options(
#     -fno-rtti -fno-exceptions -fno-threadsafe-statics
# )

add_link_options(
    # -march=armv7e-m
    # -mfpu=fpv4-sp-d16
    # -mfloat-abi=hard
    -T ${CMAKE_SOURCE_DIR}/env/STM32F401RCTx_FLASH.ld
    --specs=nano.specs
    -pipe
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)