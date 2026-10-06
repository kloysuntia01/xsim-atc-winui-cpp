#include "pch.h"

#include "wav_pcm16_reader.h"

#include <filesystem>

namespace
{
    std::filesystem::path source_root()
    {
        return std::filesystem::path{ XSIMATC_SOURCE_DIR };
    }
}

TEST(WavPcm16ReaderTests, ReadsWhisperJfkSample)
{
    const auto wav_path =
        source_root() / "external" / "whisper.cpp" / "samples" / "jfk.wav";

    ASSERT_TRUE(std::filesystem::exists(wav_path)) << wav_path.string();

    const auto audio = xsim::speech::WavPcm16Reader::read_mono_16khz(wav_path);

    EXPECT_EQ(audio.sample_rate, 16000);
    EXPECT_FALSE(audio.samples.empty());
}
