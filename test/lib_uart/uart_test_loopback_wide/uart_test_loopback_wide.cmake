#**********************
# Gather Sources
#**********************
file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
set(APP_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/..)

#**********************
# Flags
#**********************
set(APP_COMPILER_FLAGS
    -Os
    -target=XCORE-AI-EXPLORER
)
set(APP_LINK_OPTIONS
    -report
    -target=XCORE-AI-EXPLORER
)

#**********************
# Tile Targets
#**********************
if(NOT DEFINED ENV{TEST_NUM_UARTS})
    set(TEST_NUM_UARTS 1 2 3 4)
else()
    set(TEST_NUM_UARTS $ENV{TEST_NUM_UARTS})
endif()
if(NOT DEFINED ENV{TEST_BAUD})
    set(TEST_BAUD 57600 115200 230400 576000 921600)
else()
    set(TEST_BAUD $ENV{TEST_BAUD})
endif()


#**********************
# Setup targets
#**********************
foreach(num ${TEST_NUM_UARTS})
    foreach(baud ${TEST_BAUD})
        set(TARGET_NAME "test_hil_uart_loopback_wide_test_${num}_${baud}")
        # message(STATUS "${TARGET_NAME}") #Print so we can copy to build_tests sh file
        add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL)
        target_sources(${TARGET_NAME} PUBLIC ${APP_SOURCES})
        target_include_directories(${TARGET_NAME} PUBLIC ${APP_INCLUDES})
        target_compile_definitions(${TARGET_NAME}
            PRIVATE
                ${APP_COMPILE_DEFINITIONS}
                TEST_NUM_UARTS=${num}
                TEST_BAUD=${baud}
        )
        target_compile_options(${TARGET_NAME} PRIVATE ${APP_COMPILER_FLAGS})
        target_link_libraries(${TARGET_NAME} PUBLIC io::uart core::utils)
        target_link_options(${TARGET_NAME} PRIVATE ${APP_LINK_OPTIONS})
        set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
        unset(TARGET_NAME)
    endforeach()
endforeach()
