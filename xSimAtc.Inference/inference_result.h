#pragma once

#include <cstdint>
#include <string>

namespace xsim::inference
{
    enum class InferenceFinishReason
    {
        MaxTokens,
        EndOfGeneration
    };

    struct InferenceResult final
    {
        // Exact decoded model output. Preserve this for diagnostics,
        // provenance, and future model-specific handling.
        std::string raw_text;

        // Consumer-facing text after deterministic normalization.
        std::string final_text;

        std::int32_t prompt_tokens{};
        std::int32_t generated_tokens{};
        bool stopped_on_eog{};
        InferenceFinishReason finish_reason{
            InferenceFinishReason::MaxTokens };
    };

    [[nodiscard]]
    constexpr const char* inference_finish_reason_name(
        const InferenceFinishReason reason) noexcept
    {
        switch (reason)
        {
        case InferenceFinishReason::EndOfGeneration:
            return "end-of-generation";

        case InferenceFinishReason::MaxTokens:
        default:
            return "max-tokens";
        }
    }
}
