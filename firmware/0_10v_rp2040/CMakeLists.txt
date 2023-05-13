cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(pico-sdk/pico_sdk_init.cmake)

project(balyrond2040)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(0_10v_rp2040
    main.c
)

pico_generate_pio_header(0_10v_rp2040 ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

pico_enable_stdio_usb(0_10v_rp2040 1)
pico_enable_stdio_uart(0_10v_rp2040 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(0_10v_rp2040 PRIVATE pico_stdlib pico_time hardware_pio hardware_adc)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(0_10v_rp2040)

