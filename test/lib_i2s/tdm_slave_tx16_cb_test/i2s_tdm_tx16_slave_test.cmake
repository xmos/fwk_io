#**********************
# Gather Sources
#**********************
file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
set(APP_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/src)

#**********************
# Flags
#**********************
set(APP_COMPILER_FLAGS
    -O3
    -target=XCORE-AI-EXPLORER
)
set(APP_LINK_OPTIONS
    -report
    -target=XCORE-AI-EXPLORER
)

#**********************
# Tile Targets
#**********************
if(NOT DEFINED ENV{TX_OFFSET})
    set(TX_OFFSET "0" "1" "2")    
else()
    set(TX_OFFSET $ENV{TX_OFFSET})
endif()

#**********************
# Setup targets
#**********************
foreach(tx_offset ${TX_OFFSET})
    set(TARGET_NAME "test_hil_i2s_tdm_tx16_slave_test_${tx_offset}")
    add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL)
    target_sources(${TARGET_NAME} PUBLIC ${APP_SOURCES})
    target_include_directories(${TARGET_NAME} PUBLIC ${APP_INCLUDES})
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            ${APP_COMPILE_DEFINITIONS}
            TX_OFFSET=${tx_offset}
    )
    target_compile_options(${TARGET_NAME} PRIVATE ${APP_COMPILER_FLAGS})
    target_link_libraries(${TARGET_NAME} PUBLIC lib_i2s)
    target_link_options(${TARGET_NAME} PRIVATE ${APP_LINK_OPTIONS})
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
    unset(TARGET_NAME)
endforeach()
