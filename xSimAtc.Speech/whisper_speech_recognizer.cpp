#include "pch.h"
#include "whisper_speech_recognizer.h"

#include "wav_pcm16_reader.h"

#include <utility>

namespace xsim::speech
{
    WhisperSpeechRecognizer::WhisperSpeechRecognizer(WhisperModelOptions options)
        : model_(std::move(options))
    {
    }

    std::string WhisperSpeechRecognizer::transcribe(
        const std::filesystem::path& audio_file)
    {
        const auto audio = WavPcm16Reader::read_mono_16khz(audio_file);
        return model_.transcribe_pcm(audio.samples);
    }
}
