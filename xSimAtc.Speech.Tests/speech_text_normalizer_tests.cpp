#include "pch.h"

#include "speech_text_normalizer.h"

using xsimatc::speech::SpeechTextNormalizer;

TEST(SpeechTextNormalizerTests, ConvertsTextToLowercase)
{
    EXPECT_EQ(
        SpeechTextNormalizer::normalize("FEDEX SIX ZERO SIX"),
        "fedex six zero six");
}

TEST(SpeechTextNormalizerTests, CollapsesWhitespace)
{
    EXPECT_EQ(
        SpeechTextNormalizer::normalize("  fedex   six\tzero\n six  "),
        "fedex six zero six");
}

TEST(SpeechTextNormalizerTests, ReplacesPunctuationWithWordBoundaries)
{
    EXPECT_EQ(
        SpeechTextNormalizer::normalize("FDX606, cleared-to-land!"),
        "fdx606 cleared to land");
}

TEST(SpeechTextNormalizerTests, EmptyOrPunctuationOnlyTextNormalizesToEmpty)
{
    EXPECT_TRUE(SpeechTextNormalizer::normalize("").empty());
    EXPECT_TRUE(SpeechTextNormalizer::normalize("... !!! ---").empty());
}
