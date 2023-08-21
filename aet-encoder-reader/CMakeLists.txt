cmake_minimum_required(VERSION 3.12)

if (DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else()
    message(FATAL_ERROR "PICO_SDK_PATH çevresel değişkeni tanımlı değil!")
endif()

# PICO SDK'yi dahil edin
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(angle_reader)

pico_sdk_init()

add_subdirectory(src)
if (TARGET tinyusb_device)
    add_executable(main
        main.cpp
    )

    target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/include)
    target_link_libraries(main pico_stdlib aeat6600 ili934x hardware_flash 
    )
    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 0)
    pico_add_extra_outputs(main)
else()
    message(WARNING "tinyusb_device hedefi bulunamadı. angle_reader oluşturulmayacak.")
endif()
