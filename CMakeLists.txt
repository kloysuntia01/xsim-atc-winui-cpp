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

enable_testing()

add_subdirectory(xSimAtc.Aircrafts)
add_subdirectory(xSimAtc.Aircrafts.Tests)