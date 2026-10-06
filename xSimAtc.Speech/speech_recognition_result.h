#pragma once

#include <string>

namespace xsim::speech
{
    class SpeechRecognitionResult final
    {
    public:
        SpeechRecognitionResult() = default;
        explicit SpeechRecognitionResult(std::string text);

        [[nodiscard]]
        const std::string& text() const noexcept;

        [[nodiscard]]
        bool empty() const noexcept;

    private:
        std::string text_;
    };
}
