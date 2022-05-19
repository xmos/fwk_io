
add_subdirectory(i2c)
add_subdirectory(i2s)
add_subdirectory(mic_array)
add_subdirectory(qspi_io)
add_subdirectory(spi)
add_subdirectory(xud)
add_subdirectory(uart)

#if((${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A) OR (${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS2A))
#    ## Create an aggregate of hil libraries
#    add_library(xcore_sdk_hil INTERFACE)
#    target_link_libraries(xcore_sdk_hil
#        INTERFACE
#            sdk::hil::lib_i2c
#            sdk::hil::lib_qspi_io
#            sdk::hil::lib_spi
#           sdk::hil::lib_xud
#            sdk::hil::lib_uart
#    )
#    add_library(sdk::hil ALIAS xcore_sdk_hil)
#
#    ## Create an aggregate of hil libraries
#    add_library(xcore_sdk_hil_audio INTERFACE)
#    target_link_libraries(xcore_sdk_hil_audio
#        INTERFACE
#            sdk::hil::lib_mic_array
#            sdk::hil::lib_i2s
#    )
#    add_library(sdk::hil_audio ALIAS xcore_sdk_hil_audio)
#endif()
