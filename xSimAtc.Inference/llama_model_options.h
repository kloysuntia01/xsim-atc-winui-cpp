#pragma once

#include <filesystem>

namespace xsim::inference
{
    struct LlamaModelOptions final
    {
        std::filesystem::path model_path;
        int gpu_layers{ 0 };
        bool use_mmap{ true };
    };
}
