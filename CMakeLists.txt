cmake_minimum_required(VERSION 3.28)

project(
    xsimatc-cpp
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only RxCpp dependency.
add_library(rxcpp INTERFACE)

target_include_directories(
    rxcpp
    INTERFACE
        ${CMAKE_SOURCE_DIR}/external/RxCpp/Rx/v2/src
)

# Fetch GoogleTest once for the entire solution.
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

set(
    gtest_force_shared_crt
    ON
    CACHE BOOL
    ""
    FORCE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(xSimAtc.Aircrafts)
add_subdirectory(xSimAtc.Towers)
add_subdirectory(xSimAtc.Communications)
add_subdirectory(xSimAtc.Airfields)
add_subdirectory(xSimAtc.Terminal.WinUI)

add_subdirectory(xSimAtc.Aircrafts.Tests)
add_subdirectory(xSimAtc.Towers.Tests)
add_subdirectory(xSimAtc.Communications.Tests)
add_subdirectory(xSimAtc.Airfields.Tests)
