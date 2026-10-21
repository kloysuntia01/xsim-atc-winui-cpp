#include "llama_cpp_inference.h"
#include "windows_text_to_speech.h"
#include "atc_speech_formatter.h"

#include <Windows.h>
#include <conio.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace
{
    struct CliOptions final
    {
        std::string model_path{ R"(C:\AI\Models\Qwen3-0.6B-Q4_K_M.gguf)" };
        std::int32_t max_tokens{ 128 };
        bool tts_enabled{ true };
        std::string system_prompt{
            "You are the local xSimAtc assistant. Answer concisely and directly."
        };
    };

    class ConsoleInput final
    {
    public:
        ConsoleInput()
        {
            const HANDLE h = ::GetStdHandle(STD_INPUT_HANDLE);
            const DWORD type =
                (h == nullptr || h == INVALID_HANDLE_VALUE)
                    ? FILE_TYPE_UNKNOWN
                    : ::GetFileType(h);

            redirected_ =
                type == FILE_TYPE_PIPE ||
                type == FILE_TYPE_DISK;
        }

        [[nodiscard]]
        std::optional<std::string> read_line()
        {
            if (redirected_)
            {
                std::string line;
                if (!std::getline(std::cin, line))
                {
                    return std::nullopt;
                }
                return line;
            }

            return read_keyboard_line();
        }

        [[nodiscard]]
        const char* mode_name() const noexcept
        {
            return redirected_
                ? "redirected stdin"
                : "_getwch keyboard input";
        }

    private:
        [[nodiscard]]
        static std::string utf8_from_wide(const std::wstring& value)
        {
            if (value.empty())
            {
                return {};
            }

            const int bytes =
                ::WideCharToMultiByte(
                    CP_UTF8, 0,
                    value.data(),
                    static_cast<int>(value.size()),
                    nullptr, 0, nullptr, nullptr);

            if (bytes <= 0)
            {
                throw std::runtime_error("WideCharToMultiByte failed.");
            }

            std::string result(static_cast<std::size_t>(bytes), '\0');

            if (::WideCharToMultiByte(
                    CP_UTF8, 0,
                    value.data(),
                    static_cast<int>(value.size()),
                    result.data(), bytes,
                    nullptr, nullptr) != bytes)
            {
                throw std::runtime_error("WideCharToMultiByte failed.");
            }

            return result;
        }

        [[nodiscard]]
        static std::optional<std::string> read_keyboard_line()
        {
            std::wstring line;

            for (;;)
            {
                const wchar_t ch =
                    static_cast<wchar_t>(::_getwch());

                if (ch == 0 || ch == 0xE0)
                {
                    (void)::_getwch();
                    continue;
                }

                if (ch == L'\r' || ch == L'\n')
                {
                    std::wcout << L"\r\n" << std::flush;
                    break;
                }

                if (ch == 0x1A || ch == 0x04)
                {
                    if (line.empty())
                    {
                        return std::nullopt;
                    }

                    std::wcout << L"\r\n" << std::flush;
                    break;
                }

                if (ch == L'\b')
                {
                    if (!line.empty())
                    {
                        line.pop_back();
                        std::wcout << L"\b \b" << std::flush;
                    }
                    continue;
                }

                if (ch < 0x20)
                {
                    continue;
                }

                line.push_back(ch);
                std::wcout << ch << std::flush;
            }

            return utf8_from_wide(line);
        }

        bool redirected_{ false };
    };

    [[nodiscard]]
    std::string trim(std::string value)
    {
        const auto not_space =
            [](unsigned char ch) { return !std::isspace(ch); };

        value.erase(
            value.begin(),
            std::find_if(value.begin(), value.end(), not_space));

        value.erase(
            std::find_if(value.rbegin(), value.rend(), not_space).base(),
            value.end());

        return value;
    }

    [[nodiscard]]
    bool starts_with(std::string_view value, std::string_view prefix)
    {
        return value.size() >= prefix.size() &&
               value.substr(0, prefix.size()) == prefix;
    }

    void print_help()
    {
        std::cout
            << "\nCommands\n"
            << "  /help\n"
            << "  /exit\n"
            << "  /tts on\n"
            << "  /tts off\n"
            << "  /tokens <n>\n"
            << "  /system <text>\n"
            << "  /show\n\n";
    }

    void print_settings(const CliOptions& options)
    {
        std::cout
            << "\nModel:  " << options.model_path
            << "\nTTS:    " << (options.tts_enabled ? "on" : "off")
            << "\nTokens: " << options.max_tokens
            << "\nSystem: " << options.system_prompt
            << "\n\n";
    }

    void parse_args(int argc, char** argv, CliOptions& options)
    {
        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];

            if (arg == "--model" && i + 1 < argc)
            {
                options.model_path = argv[++i];
            }
            else if (arg == "--no-tts")
            {
                options.tts_enabled = false;
            }
            else if (arg == "--tokens" && i + 1 < argc)
            {
                options.max_tokens =
                    static_cast<std::int32_t>(std::stoi(argv[++i]));
            }
            else if (arg == "--help" || arg == "-h")
            {
                std::cout
                    << "xSimAtc.Inference.Cli\n"
                    << "  --model <path>\n"
                    << "  --tokens <n>\n"
                    << "  --no-tts\n";
                std::exit(0);
            }
            else
            {
                throw std::invalid_argument("Unknown CLI argument: " + arg);
            }
        }
    }
}

int main(int argc, char** argv)
{
    try
    {
        CliOptions options;
        parse_args(argc, argv, options);
        ConsoleInput input;

        std::cout
            << "xSimAtc Local Inference CLI + TTS\n"
            << "---------------------------------\n"
            << "Loading model:\n"
            << options.model_path
            << "\n\n";

        xsim::inference::LlamaCppInference inference{
            xsim::inference::LlamaModelOptions{
                .model_path = options.model_path
            }
        };

        xsimatc::speech::AtcSpeechFormatter speech_formatter;

        auto tts =
            std::make_unique<xsimatc::speech::WindowsTextToSpeech>();

        std::cout
            << "Model ready. Type /help for commands.\n"
            << "Input: " << input.mode_name() << "\n";

        while (true)
        {
            std::cout << "\nxs> " << std::flush;

            auto raw = input.read_line();
            if (!raw.has_value())
            {
                break;
            }

            auto line = trim(std::move(*raw));
            if (line.empty())
            {
                continue;
            }

            if (line == "/exit" || line == "/quit")
            {
                break;
            }

            if (line == "/help")
            {
                print_help();
                continue;
            }

            if (line == "/show")
            {
                print_settings(options);
                continue;
            }

            if (line == "/tts on")
            {
                options.tts_enabled = true;
                std::cout << "TTS enabled.\n";
                continue;
            }

            if (line == "/tts off")
            {
                options.tts_enabled = false;
                std::cout << "TTS disabled.\n";
                continue;
            }

            if (starts_with(line, "/tokens "))
            {
                options.max_tokens =
                    static_cast<std::int32_t>(
                        std::stoi(trim(line.substr(8))));
                std::cout
                    << "Max tokens: " << options.max_tokens << '\n';
                continue;
            }

            if (starts_with(line, "/system "))
            {
                options.system_prompt = trim(line.substr(8));
                std::cout << "System prompt updated.\n";
                continue;
            }

            xsim::inference::InferenceRequest request;
            request.prompt = line;
            request.generation.max_tokens = options.max_tokens;
            request.generation.context_tokens = 2048;
            request.generation.temperature = 0.0F;
            request.generation.top_k = 40;
            request.generation.top_p = 0.95F;
            request.generation.seed = 42;
            request.generation.use_chat_template = true;
            request.generation.system_prompt = options.system_prompt;

            std::cout << "\nmodel> " << std::flush;

            const auto result = inference.infer(request);
            const auto& answer =
                result.final_text.empty()
                    ? result.raw_text
                    : result.final_text;

            std::cout
                << answer
                << "\n[tokens: "
                << result.prompt_tokens
                << " + "
                << result.generated_tokens
                << ", finish: "
                << (result.stopped_on_eog
                        ? "end-of-generation"
                        : "max-tokens")
                << "]\n";

            if (options.tts_enabled && !answer.empty())
            {
                tts->speak(speech_formatter.format(answer));
            }
        }

        std::cout << "\nGoodbye.\n";
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "\nERROR: " << ex.what() << '\n';
        return 1;
    }
}
