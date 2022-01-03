cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)# Pull in SDK (must be before project)

project(jade_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C++17
pico_sdk_init()

include(FetchContent)
FetchContent_Declare(pum GIT_REPOSITORY https://github.com/AkiyukiOkayasu/pum GIT_TAG v1.0.2)
FetchContent_MakeAvailable(pum)

# SEGGER RTTのパス変数
set(RTT_DIR ${CMAKE_CURRENT_LIST_DIR}/SEGGER_RTT_V756d)

add_executable(${PROJECT_NAME}
    main.cpp
    usb_descriptors.c
    ${RTT_DIR}/RTT/SEGGER_RTT.c
    ${RTT_DIR}/RTT/SEGGER_RTT_printf.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${RTT_DIR}/Config
  ${RTT_DIR}/RTT
  )

add_compile_options(-Wall
# -Wno-format
# -Wno-unused-function
# -Wno-maybe-uninitialized
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)# Enable stdio to USB serial
pico_enable_stdio_uart(${PROJECT_NAME} 1)# Disable stdio to UART
pico_add_extra_outputs(${PROJECT_NAME})# Create map/bin/hex/uf2 file etc.
target_link_libraries(${PROJECT_NAME}     
    pico_stdlib
    hardware_i2c
    hardware_pwm
    tinyusb_device    
    pum
)