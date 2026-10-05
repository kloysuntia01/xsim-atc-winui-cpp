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

add_executable(
    xSimAtc.Towers.Tests
    tower_tests.cpp
)

target_precompile_headers(
    xSimAtc.Towers.Tests
    PRIVATE
        pch.h
)

target_link_libraries(
    xSimAtc.Towers.Tests
    PRIVATE
        xSimAtc.Towers
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(
        xSimAtc.Towers.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Towers.Tests
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()

include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Towers.Tests
)
