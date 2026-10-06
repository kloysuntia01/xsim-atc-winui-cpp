#pragma once

#include <filesystem>
#include <string>

namespace xsim::speech
{
    class ISpeechRecognizer
    {
    public:
        virtual ~ISpeechRecognizer() = default;

        [[nodiscard]]
        virtual std::string transcribe(
            const std::filesystem::path& audio_file) = 0;
    };
}
