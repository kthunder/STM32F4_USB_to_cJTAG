set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                C:/ENV/EmbeddedToolChain/riscv64-unknown/bin/riscv64-unknown-elf-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_ASM_COMPILER              ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++.exe)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}ld.exe)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy.exe)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size.exe)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-Os -g3)
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_options(-Os -g0)
endif()

add_compile_options(
    -mcpu=e902m
    -Wall 
    # -Wextra -Wpedantic
    -fdata-sections -ffunction-sections
    -pipe -MMD -MP 
    -fstrict-volatile-bitfields -fshort-enums
)

add_compile_options(
    $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>
)

add_link_options(
    -mcpu=e902m
    --specs=nosys.specs -specs=nano.specs
    -nostartfiles 
    -pipe -Wl,--gc-sections -Wl,--print-memory-usage
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -T ../support_package/soc/riscv/cw2245/compiler/gnu/cw2245_sdk.ld
)

add_link_options(
    -T../support_package/soc/riscv/cw2245/ROM/cw2245_rom_symbol_updated.txt
    -T../app/script/cw2245/cw2245_ram_retain_symbol.txt
    -T./${CMAKE_PROJECT_NAME}_symbol_from_ini.txt
)