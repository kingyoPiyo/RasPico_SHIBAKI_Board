cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
project(RasPico_SHIBAKI_Board)
set(target_name RasPico_SHIBAKI_Board)

pico_sdk_init()

add_executable(${target_name})

pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/ser_100base_fx.pio)

target_sources(${target_name} PRIVATE
    main.c
    hwinit.c
    udp.c
    vban.c
    )

target_link_libraries(${target_name} PRIVATE
    pico_stdlib
    hardware_pio
    hardware_dma
    hardware_irq
    hardware_i2c
    hardware_adc
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(${target_name} 1)
pico_enable_stdio_uart(${target_name} 0)

#pico_set_binary_type(${target_name} copy_to_ram)

pico_add_extra_outputs(${target_name})
