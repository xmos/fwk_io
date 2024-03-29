
if((${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A) OR (${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS2A))
    ## Source files
    file(GLOB_RECURSE LIB_C_SOURCES   lib_xud/lib_xud/src/*.c )
    file(GLOB_RECURSE LIB_XC_SOURCES  lib_xud/lib_xud/src/*.xc)
    file(GLOB_RECURSE LIB_ASM_SOURCES lib_xud/lib_xud/src/*.S )

    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_CrcAddrCheck.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_PidJumpTable.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_PidJumpTable_RxData.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_RxData.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_Token_In_DI.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_Token_Out_DI.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_Token_Ping.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_Token_Setup_DI.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/included/XUD_Token_SOF.S)
    list(REMOVE_ITEM LIB_ASM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_TokenJmp.S)

    ## cmake doesn't recognize .S files as assembly by default
    set_source_files_properties(LIB_ASM_SOURCES PROPERTIES LANGUAGE ASM)

    ## Assume all asm is XS3A for now
    set(XCORE_XS3A_SOURCES ${LIB_ASM_SOURCES})

    ## Includes files
    set(LIB_PUBLIC_INCLUDES
        lib_xud/lib_xud/api
        lib_xud/lib_xud/api/legacy
        lib_xud/lib_xud/src/user
        lib_xud/lib_xud/src/user/class
    )
    set(LIB_PRIVATE_INCLUDES
        lib_xud/lib_xud/src
        lib_xud/lib_xud/src/core
    )

    ## Gather library sources
    set(LIB_PUBLIC_SOURCES   "")
    set(LIB_PRIVATE_SOURCES  ${LIB_C_SOURCES} ${LIB_CXX_SOURCES} ${LIB_XC_SOURCES})

    ## Append platform specific sources
    list(APPEND LIB_PRIVATE_SOURCES ${${CMAKE_SYSTEM_NAME}_SOURCES})

    ## Apply compile flags to sources at the top level directory because this is an INTERFACE library
    set_source_files_properties(${LIB_PRIVATE_SOURCES} DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTIES COMPILE_OPTIONS "-O3;-DREF_CLK_FREQ=100;-fasm-linenum;-fcomment-asm;-DXUD_FULL_PIDTABLE=1")

    ## Append file specific compile flags to sources at the top level directory because this is an INTERFACE library
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_IoLoop.S DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-fschedule")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_Main.xc DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-mno-dual-issue")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_Support.xc DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-mno-dual-issue")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_Signalling.xc DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-Wno-return-type")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_TestMode.xc DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-mno-dual-issue")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_SetCrcTableAddr.c DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-mno-dual-issue")
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib_xud/lib_xud/src/core/XUD_HAL.xc DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-mno-dual-issue") # Not in XS3 branch

    ## Create library target
    ## INTERFACE because we depend on the board target supplied usb tile and core speed
    add_library(lib_xud INTERFACE)
    target_sources(lib_xud
        INTERFACE
            ${LIB_PUBLIC_SOURCES}
            ${LIB_PRIVATE_SOURCES}
    )
    target_include_directories(lib_xud
        INTERFACE
            ${LIB_PUBLIC_INCLUDES}
            ${LIB_PRIVATE_INCLUDES}
    )
    target_link_libraries(lib_xud
        INTERFACE
            framework_core_legacy_compat
    )
endif()
