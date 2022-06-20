add_baremetal_executable(ColorCompressor  
        "circular_buffer.h"
        "circular_buffer.c"
        "color.h"
        "color.c"
        "command.h"
        "command.c"
        "main.h"
        "main.c"
        "packet.h"
        "receiver.h"
        "receiver.c"
        "transmitter.h"
        "transmitter.c")

target_compile_features(ColorCompressor PUBLIC c_std_99)

target_compile_options(ColorCompressor PRIVATE
        ${BASIC_COMPILE_OPTIONS}
        ${BUILD_TYPE_SPECIFIC_COMPILE_OPTIONS})

target_link_options(ColorCompressor PRIVATE
        -T ${LINKER_SCRIPT}
        ${BASIC_LINK_OPTIONS}
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage)
        
target_include_directories(ColorCompressor PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ColorCompressor PRIVATE stm32::f051r8)
