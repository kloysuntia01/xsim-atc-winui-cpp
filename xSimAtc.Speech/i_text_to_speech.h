#pragma once

#include <string_view>

namespace xsimatc::speech
{
    class ITextToSpeech
    {
    public:
        virtual ~ITextToSpeech() = default;

        virtual void speak(std::string_view text) = 0;
    };
}
