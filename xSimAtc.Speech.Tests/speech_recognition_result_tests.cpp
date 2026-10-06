#include "pch.h"

#include "speech_recognition_result.h"

using xsim::speech::SpeechRecognitionResult;

namespace xsim::speech::tests
{
    TEST(SpeechRecognitionResultTests, DefaultResultIsEmpty)
    {
        const SpeechRecognitionResult result;

        EXPECT_TRUE(result.empty());
        EXPECT_TRUE(result.text().empty());
    }

    TEST(SpeechRecognitionResultTests, ConstructorPreservesRecognizedText)
    {
        const SpeechRecognitionResult result{ "FedEx six zero six cleared to land" };

        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result.text(), "FedEx six zero six cleared to land");
    }
}
