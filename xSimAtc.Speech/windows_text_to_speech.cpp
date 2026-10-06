#include "pch.h"
#include "windows_text_to_speech.h"

#include <Windows.h>
#include <sapi.h>

#include <stdexcept>
#include <string>

namespace
{
    [[nodiscard]]
    std::wstring utf8_to_wide(const std::string_view text)
    {
        if (text.empty())
        {
            return {};
        }

        const int required = ::MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            nullptr,
            0);

        if (required <= 0)
        {
            throw std::runtime_error(
                "Windows TTS received invalid UTF-8 text.");
        }

        std::wstring wide(
            static_cast<std::size_t>(required),
            L'\0');

        const int converted = ::MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            wide.data(),
            required);

        if (converted != required)
        {
            throw std::runtime_error(
                "Windows TTS could not convert UTF-8 text.");
        }

        return wide;
    }

    class ComApartment final
    {
    public:
        ComApartment()
        {
            const HRESULT result = ::CoInitializeEx(
                nullptr,
                COINIT_APARTMENTTHREADED);

            if (result == RPC_E_CHANGED_MODE)
            {
                return;
            }

            if (FAILED(result))
            {
                throw std::runtime_error(
                    "Windows TTS could not initialize COM.");
            }

            owns_initialization_ = true;
        }

        ~ComApartment()
        {
            if (owns_initialization_)
            {
                ::CoUninitialize();
            }
        }

        ComApartment(const ComApartment&) = delete;
        ComApartment& operator=(const ComApartment&) = delete;

    private:
        bool owns_initialization_{ false };
    };
}

namespace xsimatc::speech
{
    WindowsTextToSpeech::WindowsTextToSpeech(
        TextToSpeechOptions options)
        : options_(options)
    {
        validate_options(options_);
    }

    void WindowsTextToSpeech::speak(const std::string_view text)
    {
        if (text.empty())
        {
            return;
        }

        const std::wstring wide_text = utf8_to_wide(text);
        ComApartment apartment;

        ISpVoice* voice = nullptr;
        const HRESULT create_result = ::CoCreateInstance(
            CLSID_SpVoice,
            nullptr,
            CLSCTX_ALL,
            IID_ISpVoice,
            reinterpret_cast<void**>(&voice));

        if (FAILED(create_result) || voice == nullptr)
        {
            throw std::runtime_error(
                "Windows TTS could not create the local SAPI voice.");
        }

        const auto release_voice = [&voice]() noexcept
        {
            if (voice != nullptr)
            {
                voice->Release();
                voice = nullptr;
            }
        };

        HRESULT result = voice->SetRate(options_.rate);
        if (SUCCEEDED(result))
        {
            result = voice->SetVolume(options_.volume);
        }
        if (SUCCEEDED(result))
        {
            result = voice->Speak(
                wide_text.c_str(),
                SPF_DEFAULT,
                nullptr);
        }

        release_voice();

        if (FAILED(result))
        {
            throw std::runtime_error(
                "Windows TTS could not synthesize speech.");
        }
    }

    const TextToSpeechOptions&
        WindowsTextToSpeech::options() const noexcept
    {
        return options_;
    }

    void WindowsTextToSpeech::validate_options(
        const TextToSpeechOptions& options)
    {
        if (options.rate < -10 || options.rate > 10)
        {
            throw std::invalid_argument(
                "Windows TTS rate must be between -10 and 10.");
        }

        if (options.volume > 100)
        {
            throw std::invalid_argument(
                "Windows TTS volume must be between 0 and 100.");
        }
    }
}
