
cmake_minimum_required(VERSION 3.21)

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(framework_io)

## Enable languages for project
enable_language(CXX C ASM)

## Project options
option(FWK_IO_TESTS     "Enable framework_io tests"  OFF)

## Setup a framework root path
set(FRAMEWORK_IO_ROOT_PATH ${PROJECT_SOURCE_DIR} CACHE STRING "Root folder of framework_io in this cmake project tree")

## Add top level project targets
if(PROJECT_IS_TOP_LEVEL)
    ## Fetch dependencies if top level
    include(tools/ci/fetch_ci_deps.cmake)
endif()

add_subdirectory(modules)

if(FWK_IO_TESTS)
    include(test/tests.cmake)
endif()
