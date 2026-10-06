#include "pch.h"

#include "speech_recognition_result.h"

#include <utility>

namespace xsim::speech
{
    SpeechRecognitionResult::SpeechRecognitionResult(std::string text)
        : text_(std::move(text))
    {
    }

    const std::string& SpeechRecognitionResult::text() const noexcept
    {
        return text_;
    }

    bool SpeechRecognitionResult::empty() const noexcept
    {
        return text_.empty();
    }
}
