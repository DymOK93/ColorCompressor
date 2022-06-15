cmake_minimum_required(VERSION 3.15)
project(ColorCompressor)
enable_language(C ASM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/elf")
set(LINKER_SCRIPT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/link")

set(ARCHITECTURE_SPECIFIC_OPTIONS
        -mcpu=cortex-m0
        -mthumb
        -mfpu=auto
        -mfloat-abi=soft)

set(BASIC_COMPILE_OPTIONS
        ${ARCHITECTURE_SPECIFIC_OPTIONS}
        -fdata-sections
        -ffunction-sections
        -Wall)

set(OPT_COMPILE_OPTIONS
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:Release>:-O3 -flto>)

set(BASIC_LINK_OPTIONS
        ${ARCHITECTURE_SPECIFIC_OPTIONS}
        -specs=nano.specs
        -lc
        -lm
        -lnosys)

set(LINKER_SCRIPT "${LINKER_SCRIPT_DIRECTORY}/STM32F051R8_FLASH.lds")

add_subdirectory(stm32)

add_baremetal_executable(ColorCompressor  
        "circular_buffer.h"
        "circular_buffer.c"
        "color.h"
        "color.c"
        "command.h"
        "command.c"
        "main.c"
        "packet.h"
        "receiver.h"
        "receiver.c"
        "transmitter.h"
        "transmitter.c")

target_compile_features(ColorCompressor PUBLIC c_std_99)

target_compile_options(ColorCompressor PRIVATE
        ${BASIC_COMPILE_OPTIONS}
        ${OPT_COMPILE_OPTIONS})

target_link_options(ColorCompressor PRIVATE
        -T ${LINKER_SCRIPT}
        ${BASIC_LINK_OPTIONS}
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage)
        
target_include_directories(ColorCompressor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ColorCompressor PUBLIC stm32::f051r8)


