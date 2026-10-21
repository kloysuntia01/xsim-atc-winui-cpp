#pragma once

#include <cstdint>
#include <string>

namespace xsim::inference
{
    struct GenerationOptions final
    {
        std::int32_t max_tokens{ 32 };
        std::uint32_t context_tokens{ 2048 };

        // Zero selects deterministic greedy decoding.
        float temperature{ 0.0F };

        // Applied only when temperature > 0.
        std::int32_t top_k{ 40 };
        float top_p{ 0.95F };

        // Fixed default keeps non-greedy smoke/debug runs reproducible.
        std::uint32_t seed{ 42 };

        // When enabled, use the model's GGUF chat template and append an
        // assistant-generation marker before tokenization.
        bool use_chat_template{ false };

        // Optional system message used only with chat-template mode.
        std::string system_prompt;
    };
}
