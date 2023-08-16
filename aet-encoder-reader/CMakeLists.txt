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

if (TARGET tinyusb_device)
    add_executable(angle_reader
        src/angle_reader.c
    )
    
    target_link_libraries(angle_reader pico_stdlib)
    pico_enable_stdio_usb(angle_reader 1)
    pico_enable_stdio_uart(angle_reader 0)
    pico_add_extra_outputs(angle_reader)
else()
    message(WARNING "tinyusb_device hedefi bulunamadı. angle_reader oluşturulmayacak.")
endif()
