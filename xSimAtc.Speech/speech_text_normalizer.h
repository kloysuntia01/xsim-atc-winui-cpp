#pragma once

#include <string>
#include <string_view>

namespace xsimatc::speech
{
    class SpeechTextNormalizer final
    {
    public:
        [[nodiscard]]
        static std::string normalize(std::string_view text);
    };
}
