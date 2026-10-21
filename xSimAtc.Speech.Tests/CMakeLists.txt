add_executable(
    xSimAtc.Speech.Tests
    atc_command_parser_tests.cpp
    speech_recognition_result_tests.cpp
    speech_text_normalizer_tests.cpp
    wav_pcm16_reader_tests.cpp
    whisper_model_tests.cpp
    whisper_speech_recognizer_tests.cpp
    windows_text_to_speech_tests.cpp
)

target_link_libraries(
    xSimAtc.Speech.Tests
    PRIVATE
        xSimAtc.Speech
        GTest::gtest_main
)

target_include_directories(
    xSimAtc.Speech.Tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/xSimAtc.Speech
)

target_compile_definitions(
    xSimAtc.Speech.Tests
    PRIVATE
        XSIMATC_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

if(MSVC)
    target_compile_options(
        xSimAtc.Speech.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Speech.Tests
)

# LLAMA-001H650U: Standalone ATC speech formatter regression.
# Separate executable because its source defines main().
add_executable(
    xSimAtc.Speech.Formatter.Tests
    atc_speech_formatter_tests.cpp
)

target_link_libraries(
    xSimAtc.Speech.Formatter.Tests
    PRIVATE
        xSimAtc.Speech
)

target_compile_features(
    xSimAtc.Speech.Formatter.Tests
    PRIVATE
        cxx_std_23
)

add_test(
    NAME xSimAtc.Speech.Formatter.Tests
    COMMAND xSimAtc.Speech.Formatter.Tests
)
