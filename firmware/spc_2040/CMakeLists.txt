cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(pico-sdk/pico_sdk_init.cmake)

project(2040spc)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(xiao2040spc
    main.c
)

pico_generate_pio_header(xiao2040spc ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

pico_enable_stdio_usb(xiao2040spc 1)
pico_enable_stdio_uart(xiao2040spc 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(xiao2040spc PRIVATE pico_stdlib hardware_pio)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(xiao2040spc)

