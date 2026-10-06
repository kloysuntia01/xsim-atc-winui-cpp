#pragma once

#include "i_speech_recognizer.h"
#include "whisper_model.h"

namespace xsim::speech
{
    class WhisperSpeechRecognizer final : public ISpeechRecognizer
    {
    public:
        explicit WhisperSpeechRecognizer(WhisperModelOptions options);

        [[nodiscard]]
        std::string transcribe(
            const std::filesystem::path& audio_file) override;

    private:
        WhisperModel model_;
    };
}
