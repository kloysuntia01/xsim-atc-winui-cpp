#pragma once

#include <filesystem>
#include <vector>

namespace xsim::speech
{
    struct WavPcm16Audio final
    {
        int sample_rate{};
        std::vector<float> samples;
    };

    class WavPcm16Reader final
    {
    public:
        [[nodiscard]]
        static WavPcm16Audio read_mono_16khz(
            const std::filesystem::path& audio_file);
    };
}
