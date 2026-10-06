#include "pch.h"

#include "whisper_speech_recognizer.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>

namespace
{
    std::filesystem::path source_root()
    {
        return std::filesystem::path{ XSIMATC_SOURCE_DIR };
    }

    std::string lower_copy(std::string value)
    {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char value)
            {
                return static_cast<char>(std::tolower(value));
            });

        return value;
    }
}

TEST(WhisperSpeechRecognizerTests, TranscribesKnownJfkSample)
{
    const auto model_path =
        source_root() / "external" / "whisper.cpp" / "models" / "ggml-tiny.en.bin";
    const auto wav_path =
        source_root() / "external" / "whisper.cpp" / "samples" / "jfk.wav";

    ASSERT_TRUE(std::filesystem::exists(model_path))
        << "Download ggml-tiny.en.bin before running this integration test: "
        << model_path.string();
    ASSERT_TRUE(std::filesystem::exists(wav_path)) << wav_path.string();

    xsim::speech::WhisperModelOptions options;
    options.model_path = model_path;
    options.language = "en";
    options.thread_count = 4;

    xsim::speech::WhisperSpeechRecognizer recognizer{ std::move(options) };

    const auto transcript = lower_copy(recognizer.transcribe(wav_path));

    EXPECT_NE(transcript.find("fellow americans"), std::string::npos)
        << transcript;
}
