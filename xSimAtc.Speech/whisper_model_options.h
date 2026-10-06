#pragma once

#include <filesystem>
#include <string>

namespace xsim::speech
{
    struct WhisperModelOptions final
    {
        std::filesystem::path model_path;
        std::string language{ "en" };
        int thread_count{ 4 };
    };
}
