#pragma once

#include <cstdint>

namespace xsimatc::speech
{
    struct TextToSpeechOptions final
    {
        // Native Windows SAPI rate range is -10 through +10.
        int rate{ 0 };

        // Native Windows SAPI volume range is 0 through 100.
        std::uint16_t volume{ 100 };
    };
}
