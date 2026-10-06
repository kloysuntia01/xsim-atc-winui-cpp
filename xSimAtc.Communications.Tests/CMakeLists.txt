add_executable(
    xSimAtc.Communications.Tests
    communication_checkpoint_tests.cpp
    tracking_state_tests.cpp
)

target_precompile_headers(
    xSimAtc.Communications.Tests
    PRIVATE
        pch.h
)

target_link_libraries(
    xSimAtc.Communications.Tests
    PRIVATE
        xSimAtc.Communications
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(
        xSimAtc.Communications.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Communications.Tests
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()

include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Communications.Tests
)
