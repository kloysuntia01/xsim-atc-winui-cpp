#include "pch.h"
#include "atc_command_parser.h"

#include <array>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace
{
    struct ParsedAction final
    {
        xsimatc::speech::AtcAction action{
            xsimatc::speech::AtcAction::Unknown
        };

        std::size_t next_index{};
    };

    [[nodiscard]]
    std::vector<std::string> split_words(
        const std::string_view text)
    {
        std::vector<std::string> words;
        std::string current;

        for (const char value : text)
        {
            if (std::isspace(
                    static_cast<unsigned char>(value)) != 0)
            {
                if (!current.empty())
                {
                    words.push_back(current);
                    current.clear();
                }

                continue;
            }

            current.push_back(value);
        }

        if (!current.empty())
        {
            words.push_back(current);
        }

        return words;
    }

    [[nodiscard]]
    bool matches_phrase(
        const std::vector<std::string>& words,
        const std::size_t start,
        const std::array<std::string_view, 3>& phrase)
    {
        if (start + phrase.size() > words.size())
        {
            return false;
        }

        for (std::size_t index = 0;
             index < phrase.size();
             ++index)
        {
            if (words[start + index] != phrase[index])
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]]
    std::optional<ParsedAction> parse_action(
        const std::vector<std::string>& words,
        const std::size_t start)
    {
        using xsimatc::speech::AtcAction;

        constexpr std::array<std::string_view, 3> cleared_to_land{
            "cleared",
            "to",
            "land"
        };

        constexpr std::array<std::string_view, 3> cleared_for_takeoff{
            "cleared",
            "for",
            "takeoff"
        };

        if (matches_phrase(
                words,
                start,
                cleared_to_land))
        {
            return ParsedAction{
                .action = AtcAction::ClearedToLand,
                .next_index = start + cleared_to_land.size()
            };
        }

        if (matches_phrase(
                words,
                start,
                cleared_for_takeoff))
        {
            return ParsedAction{
                .action = AtcAction::ClearedForTakeoff,
                .next_index = start + cleared_for_takeoff.size()
            };
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<char> spoken_digit(
        const std::string_view word)
    {
        static const std::unordered_map<std::string_view, char> digits{
            { "zero", '0' },
            { "one", '1' },
            { "two", '2' },
            { "three", '3' },
            { "four", '4' },
            { "five", '5' },
            { "six", '6' },
            { "seven", '7' },
            { "eight", '8' },
            { "nine", '9' }
        };

        const auto found = digits.find(word);

        if (found == digits.end())
        {
            return std::nullopt;
        }

        return found->second;
    }

    [[nodiscard]]
    std::optional<std::string> parse_call_sign(
        const std::vector<std::string>& words,
        std::size_t& action_start)
    {
        if (words.empty())
        {
            return std::nullopt;
        }

        if (words[0].starts_with("fdx") &&
            words[0].size() > 3)
        {
            action_start = 1;

            std::string call_sign = words[0];

            for (char& value : call_sign)
            {
                value = static_cast<char>(
                    std::toupper(
                        static_cast<unsigned char>(value)));
            }

            return call_sign;
        }

        if (words[0] != "fedex")
        {
            return std::nullopt;
        }

        std::string digits;
        std::size_t index = 1;

        while (index < words.size())
        {
            const auto digit =
                spoken_digit(words[index]);

            if (!digit.has_value())
            {
                break;
            }

            digits.push_back(*digit);
            ++index;
        }

        if (digits.empty())
        {
            return std::nullopt;
        }

        action_start = index;

        return "FDX" + digits;
    }

    [[nodiscard]]
    std::optional<std::string> parse_runway(
        const std::vector<std::string>& words,
        const std::size_t start)
    {
        if (start == words.size())
        {
            return std::string{};
        }

        if (words[start] != "runway")
        {
            return std::nullopt;
        }

        std::size_t index = start + 1;

        if (index >= words.size())
        {
            return std::nullopt;
        }

        std::string runway_number;

        while (index < words.size())
        {
            const auto digit =
                spoken_digit(words[index]);

            if (digit.has_value())
            {
                runway_number.push_back(*digit);
                ++index;
                continue;
            }

            bool numeric_token = !words[index].empty();

            for (const char value : words[index])
            {
                if (std::isdigit(
                        static_cast<unsigned char>(value)) == 0)
                {
                    numeric_token = false;
                    break;
                }
            }

            if (numeric_token)
            {
                runway_number += words[index];
                ++index;
            }

            break;
        }

        if (runway_number.empty() ||
            runway_number.size() > 2)
        {
            return std::nullopt;
        }

        std::string runway = runway_number;

        if (index < words.size())
        {
            if (words[index] == "left")
            {
                runway.push_back('L');
                ++index;
            }
            else if (words[index] == "right")
            {
                runway.push_back('R');
                ++index;
            }
            else if (words[index] == "center")
            {
                runway.push_back('C');
                ++index;
            }
            else
            {
                return std::nullopt;
            }
        }

        if (index != words.size())
        {
            return std::nullopt;
        }

        return runway;
    }
}

namespace xsimatc::speech
{
    std::optional<AtcCommand> AtcCommandParser::parse(
        const std::string_view normalized_text) const
    {
        const auto words =
            split_words(normalized_text);

        std::size_t action_start = 0;

        const auto call_sign =
            parse_call_sign(
                words,
                action_start);

        if (!call_sign.has_value())
        {
            return std::nullopt;
        }

        const auto parsed_action =
            parse_action(
                words,
                action_start);

        if (!parsed_action.has_value())
        {
            return std::nullopt;
        }

        const auto runway =
            parse_runway(
                words,
                parsed_action->next_index);

        if (!runway.has_value())
        {
            return std::nullopt;
        }

        return AtcCommand{
            .call_sign = *call_sign,
            .action = parsed_action->action,
            .runway = runway->empty()
                ? std::nullopt
                : std::optional<std::string>{ *runway }
        };
    }
}
