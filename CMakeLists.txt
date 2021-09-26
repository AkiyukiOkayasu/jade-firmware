cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)# Pull in SDK (must be before project)

project(example_project_name C CXX ASM)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C++17
pico_sdk_init()

add_executable(jade_rp2040
    main.cpp
    usb_descriptors.c
)

target_include_directories(jade_rp2040 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  )

add_compile_options(-Wall
# -Wno-format
# -Wno-unused-function
# -Wno-maybe-uninitialized
)

pico_enable_stdio_usb(jade_rp2040 0)# Enable stdio to USB serial
pico_enable_stdio_uart(jade_rp2040 1)# Disable stdio to UART
pico_add_extra_outputs(jade_rp2040)# Create map/bin/hex/uf2 file etc.
target_link_libraries(jade_rp2040 
    pico_stdlib
    tinyusb_device
)