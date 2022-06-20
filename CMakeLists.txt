cmake_minimum_required(VERSION 3.15)
project(ColorCompressor)
enable_language(C ASM)

set(CC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${CC_DIR}/tools/cmake")
set(LINKER_SCRIPT "${CC_DIR}/tools/link/STM32F051R8_FLASH.lds")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CC_DIR}/build")

set(ARCHITECTURE_SPECIFIC_OPTIONS
        -mcpu=cortex-m0
        -mthumb
        -mfpu=auto
        -mfloat-abi=soft)

set(BASIC_COMPILE_OPTIONS
        ${ARCHITECTURE_SPECIFIC_OPTIONS}
        -fdata-sections -ffunction-sections
        -Wall -Wextra -Wpedantic 
        -Wduplicated-branches -Wduplicated-cond 
        -Wlogical-op
        -Wcast-qual 
        #-Wconversion -Wsign-conversion
        -Werror)

set(BUILD_TYPE_SPECIFIC_COMPILE_OPTIONS
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:Release>:-O0 -g3>)

set(BASIC_LINK_OPTIONS
        ${ARCHITECTURE_SPECIFIC_OPTIONS}
        -nostartfiles
        -specs=nano.specs
        -lc -lm -lnosys)

add_subdirectory(external)
add_subdirectory(src)



