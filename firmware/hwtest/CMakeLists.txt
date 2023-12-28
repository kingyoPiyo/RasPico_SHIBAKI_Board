cmake_minimum_required(VERSION 3.12)

set(target_name hwtest)

include(pico_sdk_import.cmake)
project(${target_name})

pico_sdk_init()

add_executable(${target_name})

pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/ser_100base_fx.pio)
pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/ser_10b.pio)
pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/rx.pio)

target_sources(${target_name} PRIVATE
    main.c
    hwinit.c
    hwtest.c
    udp.c
    tbl_8b10b.c
    rx_func.c
    )

target_link_libraries(${target_name} PRIVATE
    pico_stdlib
    hardware_pio
    pico_multicore
    hardware_dma
    hardware_irq
    hardware_i2c
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(${target_name} 1)
pico_enable_stdio_uart(${target_name} 0)

pico_set_binary_type(${target_name} copy_to_ram)

pico_add_extra_outputs(${target_name})
