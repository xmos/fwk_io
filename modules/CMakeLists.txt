
add_subdirectory(i2c)
add_subdirectory(i2s)
if(${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A)
    add_subdirectory(mic_array)
endif()
add_subdirectory(qspi_io)
add_subdirectory(spi)
add_subdirectory(xud)
add_subdirectory(uart)

if((${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A) OR (${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS2A))
    ## Alias the mic array repo library target

    ## TEMPORARY hack until lib_mic_array repo is updated
    add_library(xcore_sdk_lib_xs3_math ALIAS framework_core_xs3_math)

    add_library(io::mic_array ALIAS lib_mic_array)

    ## Create an aggregate of framework_io general libraries
    add_library(framework_io_general INTERFACE)
    target_link_libraries(framework_io_general
        INTERFACE
            io::i2c
            io::qspi_io
            io::spi
            io::uart
            io::xud
    )
    add_library(io::general ALIAS framework_io_general)

    ## Create an aggregate of framework_io audio libraries
    add_library(framework_io_audio INTERFACE)
    target_link_libraries(framework_io_audio
        INTERFACE
            io::i2s
            io::mic_array
    )
    add_library(io::audio ALIAS framework_io_audio)

    ## Create an aggregate of all framework_io libraries
    add_library(framework_io_all INTERFACE)
    target_link_libraries(framework_io_all
        INTERFACE
            io::general
            io::audio
    )
    add_library(io::all ALIAS framework_io_all)
endif()