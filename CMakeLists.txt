
cmake_minimum_required(VERSION 3.21)

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(framework_io)

## Enable languages for project
enable_language(CXX C ASM)

## Setup a framework root path
set(FRAMEWORK_IO_ROOT_PATH ${PROJECT_SOURCE_DIR} CACHE STRING "Root folder of framework_io in this cmake project tree")

add_subdirectory(i2c)
add_subdirectory(i2s)
add_subdirectory(mic_array)
add_subdirectory(qspi_io)
add_subdirectory(spi)
add_subdirectory(xud)
add_subdirectory(uart)

if((${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A) OR (${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS2A))
    ## Create an aggregate of hil libraries
    add_library(xcore_sdk_hil INTERFACE)
    target_link_libraries(xcore_sdk_hil
        INTERFACE
            sdk::hil::lib_i2c
            sdk::hil::lib_qspi_io
            sdk::hil::lib_spi
            sdk::hil::lib_xud
            sdk::hil::lib_uart
    )
    add_library(sdk::hil ALIAS xcore_sdk_hil)

    ## Create an aggregate of hil libraries
    add_library(xcore_sdk_hil_audio INTERFACE)
    target_link_libraries(xcore_sdk_hil_audio
        INTERFACE
            sdk::hil::lib_mic_array
            sdk::hil::lib_i2s
    )
    add_library(sdk::hil_audio ALIAS xcore_sdk_hil_audio)

    add_library( sdk::hil::lib_mic_array ALIAS lib_mic_array)

endif()
