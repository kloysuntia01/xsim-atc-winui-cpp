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
    xSimAtc.Aircrafts.Tests
    aircraft_tests.cpp
    transponder_tests.cpp
)

target_link_libraries(
    xSimAtc.Aircrafts.Tests
    PRIVATE
        xSimAtc.Aircrafts
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(
        xSimAtc.Aircrafts.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Aircrafts.Tests
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()

include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Aircrafts.Tests
)