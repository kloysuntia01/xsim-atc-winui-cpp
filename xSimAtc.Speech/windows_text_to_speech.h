#pragma once

#include "i_text_to_speech.h"
#include "text_to_speech_options.h"

namespace xsimatc::speech
{
    class WindowsTextToSpeech final : public ITextToSpeech
    {
    public:
        explicit WindowsTextToSpeech(
            TextToSpeechOptions options = {});

        void speak(std::string_view text) override;

        [[nodiscard]]
        const TextToSpeechOptions& options() const noexcept;

    private:
        static void validate_options(
            const TextToSpeechOptions& options);

        TextToSpeechOptions options_;
    };
}
