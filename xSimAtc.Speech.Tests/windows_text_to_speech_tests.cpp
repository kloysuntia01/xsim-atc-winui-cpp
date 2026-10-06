#include "pch.h"

#include "windows_text_to_speech.h"

using xsimatc::speech::TextToSpeechOptions;
using xsimatc::speech::WindowsTextToSpeech;

TEST(WindowsTextToSpeechTests, DefaultRateIsNeutral)
{
    const WindowsTextToSpeech tts;

    EXPECT_EQ(tts.options().rate, 0);
}

TEST(WindowsTextToSpeechTests, DefaultVolumeIsFull)
{
    const WindowsTextToSpeech tts;

    EXPECT_EQ(tts.options().volume, 100);
}

TEST(WindowsTextToSpeechTests, RejectsRateOutsideSapiRange)
{
    TextToSpeechOptions options;
    options.rate = 11;

    EXPECT_THROW(
        WindowsTextToSpeech{ options },
        std::invalid_argument);
}

TEST(WindowsTextToSpeechTests, RejectsVolumeOutsideSapiRange)
{
    TextToSpeechOptions options;
    options.volume = 101;

    EXPECT_THROW(
        WindowsTextToSpeech{ options },
        std::invalid_argument);
}

TEST(WindowsTextToSpeechTests, EmptyTextDoesNotTouchSpeechEngine)
{
    WindowsTextToSpeech tts;

    EXPECT_NO_THROW(tts.speak(""));
}
