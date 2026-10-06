add_library(
    xSimAtc.Speech
    STATIC
    atc_command_parser.cpp
    speech_recognition_result.cpp
    speech_text_normalizer.cpp
    wav_pcm16_reader.cpp
    whisper_model.cpp
    whisper_speech_recognizer.cpp
    windows_text_to_speech.cpp
)

target_include_directories(
    xSimAtc.Speech
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(
    xSimAtc.Speech
    PRIVATE
        pch.h
)

target_link_libraries(
    xSimAtc.Speech
    PRIVATE
        whisper
        sapi
        ole32
)

if(MSVC)
    target_compile_options(
        xSimAtc.Speech
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

# Manual audible smoke target.
# Intentionally not registered with CTest, so automated regression runs stay silent.
add_executable(
    xSimAtc.Speech.Smoke
    Smoke/main.cpp
)

target_link_libraries(
    xSimAtc.Speech.Smoke
    PRIVATE
        xSimAtc.Speech
)

if(MSVC)
    target_compile_options(
        xSimAtc.Speech.Smoke
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()