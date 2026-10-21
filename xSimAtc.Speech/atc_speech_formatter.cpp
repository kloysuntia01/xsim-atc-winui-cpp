#include "atc_speech_formatter.h"

#include <array>
#include <cctype>
#include <regex>
#include <string>

namespace
{
    [[nodiscard]]
    const char* phonetic_for(char ch)
    {
        static constexpr std::array<const char*, 26> words{
            "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot",
            "Golf", "Hotel", "India", "Juliett", "Kilo", "Lima",
            "Mike", "November", "Oscar", "Papa", "Quebec", "Romeo",
            "Sierra", "Tango", "Uniform", "Victor", "Whiskey", "X-ray",
            "Yankee", "Zulu"
        };

        return words[static_cast<std::size_t>(ch - 'A')];
    }

    [[nodiscard]]
    const char* digit_for(char ch)
    {
        static constexpr std::array<const char*, 10> words{
            "Zero", "One", "Two", "Three", "Four",
            "Five", "Six", "Seven", "Eight", "Nine"
        };

        return words[static_cast<std::size_t>(ch - '0')];
    }

    [[nodiscard]]
    std::string spell_callsign(std::string_view token)
    {
        std::string result;

        for (const char ch : token)
        {
            if (!result.empty())
            {
                result += ' ';
            }

            if (ch >= 'A' && ch <= 'Z')
            {
                result += phonetic_for(ch);
            }
            else if (ch >= '0' && ch <= '9')
            {
                result += digit_for(ch);
            }
        }

        return result;
    }

    [[nodiscard]]
    std::string speak_runway_number(std::string_view digits)
    {
        std::string result;

        for (const char ch : digits)
        {
            if (!result.empty())
            {
                result += ' ';
            }

            result += digit_for(ch);
        }

        return result;
    }

    [[nodiscard]]
    std::string runway_suffix(char suffix)
    {
        switch (suffix)
        {
        case 'L':
            return "Left";
        case 'R':
            return "Right";
        case 'C':
            return "Center";
        default:
            return {};
        }
    }

    [[nodiscard]]
    bool looks_like_callsign(std::string_view token)
    {
        bool has_letter = false;
        bool has_digit = false;

        for (const char ch : token)
        {
            if (ch >= 'A' && ch <= 'Z')
            {
                has_letter = true;
            }
            else if (ch >= '0' && ch <= '9')
            {
                has_digit = true;
            }
            else
            {
                return false;
            }
        }

        return has_letter && has_digit;
    }
}

namespace xsimatc::speech
{
    std::string AtcSpeechFormatter::format(std::string_view text) const
    {
        std::string result{ text };

        // Runways first so 18R is not treated as a generic callsign-like token.
        static const std::regex runway_pattern{
            R"(\bRunway\s+([0-9]{1,2})([LRC])?\b)",
            std::regex_constants::icase
        };

        {
            std::string replaced;
            std::sregex_iterator it{
                result.begin(),
                result.end(),
                runway_pattern
            };
            const std::sregex_iterator end{};

            std::size_t cursor = 0;

            for (; it != end; ++it)
            {
                const auto& match = *it;

                replaced.append(
                    result,
                    cursor,
                    static_cast<std::size_t>(match.position()) - cursor);

                const std::string digits = match[1].str();
                const std::string suffix = match[2].str();

                replaced += "Runway ";
                replaced += speak_runway_number(digits);

                if (!suffix.empty())
                {
                    replaced += ' ';
                    replaced += runway_suffix(
                        static_cast<char>(
                            std::toupper(
                                static_cast<unsigned char>(suffix.front()))));
                }

                cursor =
                    static_cast<std::size_t>(
                        match.position() + match.length());
            }

            replaced.append(result, cursor, std::string::npos);

            if (!replaced.empty())
            {
                result = std::move(replaced);
            }
        }

        // Conservative callsign rule:
        // uppercase alphanumeric token containing both letters and digits.
        static const std::regex token_pattern{
            R"(\b[A-Z0-9]+\b)"
        };

        {
            std::string replaced;
            std::sregex_iterator it{
                result.begin(),
                result.end(),
                token_pattern
            };
            const std::sregex_iterator end{};

            std::size_t cursor = 0;

            for (; it != end; ++it)
            {
                const auto& match = *it;
                const std::string token = match.str();

                replaced.append(
                    result,
                    cursor,
                    static_cast<std::size_t>(match.position()) - cursor);

                if (looks_like_callsign(token))
                {
                    replaced += spell_callsign(token);
                }
                else
                {
                    replaced += token;
                }

                cursor =
                    static_cast<std::size_t>(
                        match.position() + match.length());
            }

            replaced.append(result, cursor, std::string::npos);

            if (!replaced.empty())
            {
                result = std::move(replaced);
            }
        }

        return result;
    }
}
