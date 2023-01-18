
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
    include(FetchContent)
    FetchContent_Declare(
      core
      GIT_REPOSITORY https://github.com/xmos/fwk_core.git
      GIT_TAG        61ef20e2b5b79898a9d1d095cc9f77b2906c9a06
      GIT_SUBMODULES "."
      GIT_SUBMODULES_RECURSE true
    )
    FetchContent_MakeAvailable(core)
endif()

add_subdirectory(modules)

if(FWK_IO_TESTS)
    include(test/tests.cmake)
endif()
