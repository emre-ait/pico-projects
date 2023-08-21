cmake_minimum_required(VERSION 3.12)

if (DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else()
    message(FATAL_ERROR "PICO_SDK_PATH çevresel değişkeni tanımlı değil!")
endif()

# PICO SDK'yi dahil edin
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(aeat6600)

pico_sdk_init()
add_library(aeat6600 aeat6600.cpp)
    
target_include_directories(aeat6600 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # Specify include directories

target_link_libraries(aeat6600 pico_stdlib hardware_gpio)


add_subdirectory(fonts)

project(ili934x)

add_library(ili934x ili934x.cpp)
target_include_directories(ili934x PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # Specify include directories
target_link_libraries(ili934x pico_stdlib hardware_spi  hardware_gpio fonts)