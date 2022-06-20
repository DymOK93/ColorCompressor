add_library(stm32_f051r8 STATIC 
        "core_cm0.h"
	    "core_cmInstr.h"
	    "core_cmFunc.h"
        "stm32f0xx.h"
        "system_stm32f0xx.h"
        "init.c"
        "startup_stm32f051x8.s"
        "system_stm32f0xx.c")

add_library(stm32::f051r8 ALIAS stm32_f051r8)

target_compile_definitions(stm32_f051r8 PUBLIC 
        STM32F051
        STM32F051x8)

target_compile_features(stm32_f051r8 PUBLIC c_std_99)

target_compile_options(stm32_f051r8 PUBLIC
        ${BASIC_COMPILE_OPTIONS}
        ${BUILD_TYPE_SPECIFIC_COMPILE_OPTIONS})

target_link_options(stm32_f051r8 PUBLIC
        ${BASIC_LINK_OPTIONS})

target_include_directories(stm32_f051r8 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})