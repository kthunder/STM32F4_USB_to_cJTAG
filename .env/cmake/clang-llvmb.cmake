set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                C:/ENV/EmbeddedToolChain/LLVM-ET-Arm/bin/)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}clang.exe)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}clang++.exe)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}clang)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}llvm-objcopy.exe)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}llvm-size.exe)


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
    --target=arm-none-eabi -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -mthumb
    -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections
)

add_compile_options(
    $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>
)

add_link_options(
    --target=arm-none-eabi -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -T ${CMAKE_SOURCE_DIR}/env/STM32F401RCTx_FLASH.ld
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -pipe
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)