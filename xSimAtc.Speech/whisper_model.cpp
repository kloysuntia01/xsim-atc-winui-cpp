#include "pch.h"
#include "whisper_model.h"

#include <whisper.h>

#include <stdexcept>
#include <utility>

namespace xsim::speech
{
    struct WhisperModel::Impl final
    {
        explicit Impl(whisper_context* context) noexcept
            : context(context)
        {
        }

        ~Impl()
        {
            whisper_free(context);
        }

        whisper_context* context{};
    };

    WhisperModel::WhisperModel(WhisperModelOptions options)
        : options_(std::move(options))
    {
        if (options_.model_path.empty())
        {
            throw std::invalid_argument("Whisper model path is required.");
        }

        const auto model_path = options_.model_path.string();
        auto context_params = whisper_context_default_params();

        auto* context = whisper_init_from_file_with_params(
            model_path.c_str(),
            context_params);

        if (context == nullptr)
        {
            throw std::runtime_error(
                "Unable to load Whisper model: " + model_path);
        }

        impl_ = std::make_unique<Impl>(context);
    }

    WhisperModel::~WhisperModel() = default;

    WhisperModel::WhisperModel(WhisperModel&&) noexcept = default;

    WhisperModel& WhisperModel::operator=(WhisperModel&&) noexcept = default;

    bool WhisperModel::loaded() const noexcept
    {
        return impl_ != nullptr && impl_->context != nullptr;
    }

    const WhisperModelOptions& WhisperModel::options() const noexcept
    {
        return options_;
    }

    std::string WhisperModel::transcribe_pcm(const std::vector<float>& samples)
    {
        if (!loaded())
        {
            throw std::runtime_error("Whisper model is not loaded.");
        }

        if (samples.empty())
        {
            throw std::invalid_argument("Whisper audio samples cannot be empty.");
        }

        auto params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        params.print_realtime = false;
        params.print_progress = false;
        params.print_timestamps = false;
        params.print_special = false;
        params.translate = false;
        params.language = options_.language.c_str();
        params.n_threads = options_.thread_count;

        const auto status = whisper_full(
            impl_->context,
            params,
            samples.data(),
            static_cast<int>(samples.size()));

        if (status != 0)
        {
            throw std::runtime_error("Whisper transcription failed.");
        }

        std::string transcript;
        const auto segment_count = whisper_full_n_segments(impl_->context);

        for (int i = 0; i < segment_count; ++i)
        {
            const auto* text = whisper_full_get_segment_text(impl_->context, i);
            if (text != nullptr)
            {
                transcript += text;
            }
        }

        return transcript;
    }
}
