#pragma once

#include "whisper_model_options.h"

#include <memory>
#include <string>
#include <vector>

namespace xsim::speech
{
    class WhisperModel final
    {
    public:
        explicit WhisperModel(WhisperModelOptions options);
        ~WhisperModel();

        WhisperModel(const WhisperModel&) = delete;
        WhisperModel& operator=(const WhisperModel&) = delete;

        WhisperModel(WhisperModel&&) noexcept;
        WhisperModel& operator=(WhisperModel&&) noexcept;

        [[nodiscard]]
        bool loaded() const noexcept;

        [[nodiscard]]
        const WhisperModelOptions& options() const noexcept;

        [[nodiscard]]
        std::string transcribe_pcm(const std::vector<float>& samples);

    private:
        struct Impl;

        WhisperModelOptions options_;
        std::unique_ptr<Impl> impl_;
    };
}
