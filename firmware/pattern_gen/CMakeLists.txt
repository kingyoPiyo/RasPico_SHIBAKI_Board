cmake_minimum_required(VERSION 3.12)

set(target_name pattern_gen)

include(pico_sdk_import.cmake)
project(${target_name})

pico_sdk_init()

add_executable(${target_name})

pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/ser_tx.pio)

target_sources(${target_name} PRIVATE
    main.c
    hwinit.c
    )

target_link_libraries(${target_name} PRIVATE
    pico_stdlib
    hardware_pio
    hardware_dma
    hardware_irq
    hardware_i2c
    )

pico_enable_stdio_usb(${target_name} 0)
pico_enable_stdio_uart(${target_name} 0)

pico_set_binary_type(${target_name} copy_to_ram)

pico_add_extra_outputs(${target_name})
