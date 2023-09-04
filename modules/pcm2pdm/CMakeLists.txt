cmake_minimum_required(VERSION 3.14)

set(LIB_NAME lib_delta_sigma)
set(THISLIB_PATH ${CMAKE_CURRENT_LIST_DIR})

message(STATUS "building lib_delta_sigma")

## Source files
file( GLOB_RECURSE    THISLIB_C_SOURCES       ${THISLIB_PATH}/src/*.c   )
file( GLOB_RECURSE    THISLIB_XC_SOURCES      ${THISLIB_PATH}/src/*.xc  )
file( GLOB_RECURSE    THISLIB_CPP_SOURCES     ${THISLIB_PATH}/src/*.cpp )
file( GLOB_RECURSE    THISLIB_ASM_SOURCES     ${THISLIB_PATH}/src/*.S   )

## Compile flags
unset(THISLIB_COMPILE_FLAGS)
list( APPEND  THISLIB_COMPILE_FLAGS     
              -Wno-unused-variable 
              -Wno-missing-braces
              -Wno-xcore-fptrgroup
              -O2 -g
)

# Includes
set( THISLIB_INCLUDES     ${THISLIB_PATH}/api           )

unset(THISLIB_SOURCES)
list( APPEND  THISLIB_SOURCES   ${THISLIB_C_SOURCES} 
                                ${THISLIB_XC_SOURCES}  
                                ${THISLIB_CPP_SOURCES} 
                                ${THISLIB_ASM_SOURCES} )

set_source_files_properties( ${THISLIB_XC_SOURCES}  PROPERTIES LANGUAGE C   ) 

## Apply compile flags
foreach(COMPILE_FLAG ${THISLIB_COMPILE_FLAGS})
  set_source_files_properties( ${THISLIB_SOURCES} 
        PROPERTIES COMPILE_FLAGS ${COMPILE_FLAG}) 
endforeach()


add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME}
    PRIVATE ${THISLIB_SOURCES}
)

target_include_directories(${LIB_NAME}
    PUBLIC ${THISLIB_INCLUDES}
)

target_compile_options(${LIB_NAME}
    PRIVATE ${THISLIB_COMPILE_FLAGS}
)

target_link_libraries(${LIB_NAME}
    PUBLIC
        
)
