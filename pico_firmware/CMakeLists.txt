set(PROJECT gp40_pico)

cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT} C CXX ASM)

add_executable(${PROJECT})

# Initialize the SDK
pico_sdk_init()

target_sources(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    )

add_compile_options(
    -Wall
    #-Wno-format  # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    #-Wno-unused-function # we have some for the docs that aren't called
   )
#if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
#add_compile_options(-Wno-maybe-uninitialized)
#endif()

# Make sure TinyUSB can find tusb_config.h

target_include_directories(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    )

# In addition to pico_stdlib required for common PicoSDK functionality,
# add dependency on tinyusb_host for TinyUSB device support and tinyusb_board 
# for the additional board support library used by the example

target_link_libraries(${PROJECT} PUBLIC 
    pico_stdlib pico_multicore
    tinyusb_host tinyusb_board
    )

# Create map/bin/hex file etc.

pico_add_extra_outputs(${PROJECT})

# Configure stdio

target_compile_definitions(${PROJECT} PRIVATE
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=16
    PICO_DEFAULT_UART_RX_PIN=17
    )

pico_enable_stdio_uart(${PROJECT} 1)
pico_enable_stdio_usb(${PROJECT} 0)

add_custom_target(flash COMMAND
    sudo openocd
       -f 'interface/cmsis-dap.cfg'
       -c 'transport select swd'
       -c 'adapter speed 20000'
       -f 'target/rp2040.cfg'
       -c 'program gp40_pico.elf verify reset exit'
    )
