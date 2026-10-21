#pragma once

#include <string>
#include <string_view>

namespace xsimatc::speech
{
    class AtcSpeechFormatter final
    {
    public:
        [[nodiscard]]
        std::string format(std::string_view text) const;
    };
}
