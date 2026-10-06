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

# Keep the pinned whisper.cpp dependency infrastructure-only.
# xSimAtc.Speech owns the adapter boundary; whisper headers must not leak into
# Tower, Aircraft, Airfields, or WinUI projects.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as static libraries" FORCE)
set(WHISPER_BUILD_IS_DEV OFF CACHE BOOL "Use pinned release version" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "Do not build whisper.cpp examples" FORCE)
set(WHISPER_BUILD_TESTS OFF CACHE BOOL "Do not build whisper.cpp tests" FORCE)
set(WHISPER_BUILD_SERVER OFF CACHE BOOL "Do not build whisper.cpp server" FORCE)

add_subdirectory(
    external/whisper.cpp
    external/whisper-build
    EXCLUDE_FROM_ALL
)

enable_testing()

add_subdirectory(xSimAtc.Aircrafts)
add_subdirectory(xSimAtc.Towers)
add_subdirectory(xSimAtc.Communications)
add_subdirectory(xSimAtc.Airfields)
add_subdirectory(xSimAtc.Speech)
add_subdirectory(xSimAtc.Terminal.WinUI)

add_subdirectory(xSimAtc.Aircrafts.Tests)
add_subdirectory(xSimAtc.Towers.Tests)
add_subdirectory(xSimAtc.Communications.Tests)
add_subdirectory(xSimAtc.Airfields.Tests)
add_subdirectory(xSimAtc.Speech.Tests)
