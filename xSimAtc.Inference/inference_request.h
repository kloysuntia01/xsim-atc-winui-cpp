#pragma once

#include "generation_options.h"

#include <string>

namespace xsim::inference
{
    struct InferenceRequest final
    {
        std::string prompt;
        GenerationOptions generation;
    };
}
