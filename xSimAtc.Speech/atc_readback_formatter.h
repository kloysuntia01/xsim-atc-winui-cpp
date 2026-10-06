#pragma once

#include "atc_command.h"

#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace xsimatc::speech
{
    class AtcReadbackFormatter final
    {
    public:
        [[nodiscard]]
        std::optional<std::string> format(
            const AtcCommand& command) const
        {
            const auto call_sign =
                format_call_sign(command.call_sign);

            if (!call_sign.has_value())
            {
                return std::nullopt;
            }

            const auto action =
                format_action(command.action);

            if (!action.has_value())
            {
                return std::nullopt;
            }

            std::string readback = *call_sign;

            readback += ", ";
            readback.append(
                action->data(),
                action->size());

            if (command.runway.has_value())
            {
                const auto runway =
                    format_runway(*command.runway);

                if (!runway.has_value())
                {
                    return std::nullopt;
                }

                readback += " runway ";
                readback += *runway;
            }

            readback += '.';

            return readback;
        }

    private:
        [[nodiscard]]
        static std::optional<std::string> format_call_sign(
            const std::string_view call_sign)
        {
            if (!call_sign.starts_with("FDX") ||
                call_sign.size() <= 3)
            {
                return std::nullopt;
            }

            std::string spoken = "FedEx";

            for (std::size_t index = 3;
                 index < call_sign.size();
                 ++index)
            {
                const char value = call_sign[index];

                if (std::isdigit(
                        static_cast<unsigned char>(value)) == 0)
                {
                    return std::nullopt;
                }

                const auto digit =
                    digit_word(value);

                if (!digit.has_value())
                {
                    return std::nullopt;
                }

                spoken.push_back(' ');
                spoken.append(
                    digit->data(),
                    digit->size());
            }

            return spoken;
        }

        [[nodiscard]]
        static std::optional<std::string_view> format_action(
            const AtcAction action)
        {
            switch (action)
            {
            case AtcAction::ClearedToLand:
                return "cleared to land";

            case AtcAction::ClearedForTakeoff:
                return "cleared for takeoff";

            default:
                return std::nullopt;
            }
        }

        [[nodiscard]]
        static std::optional<std::string> format_runway(
            const std::string_view runway)
        {
            if (runway.empty())
            {
                return std::nullopt;
            }

            std::string spoken;
            std::size_t index = 0;

            while (index < runway.size() &&
                   std::isdigit(
                       static_cast<unsigned char>(
                           runway[index])) != 0)
            {
                const auto digit =
                    digit_word(runway[index]);

                if (!digit.has_value())
                {
                    return std::nullopt;
                }

                if (!spoken.empty())
                {
                    spoken.push_back(' ');
                }

                spoken.append(
                    digit->data(),
                    digit->size());

                ++index;
            }

            if (spoken.empty())
            {
                return std::nullopt;
            }

            if (index < runway.size())
            {
                if (index + 1 != runway.size())
                {
                    return std::nullopt;
                }

                spoken.push_back(' ');

                switch (runway[index])
                {
                case 'L':
                    spoken += "left";
                    break;

                case 'R':
                    spoken += "right";
                    break;

                case 'C':
                    spoken += "center";
                    break;

                default:
                    return std::nullopt;
                }
            }

            return spoken;
        }

        [[nodiscard]]
        static std::optional<std::string_view> digit_word(
            const char digit)
        {
            switch (digit)
            {
            case '0':
                return "zero";
            case '1':
                return "one";
            case '2':
                return "two";
            case '3':
                return "three";
            case '4':
                return "four";
            case '5':
                return "five";
            case '6':
                return "six";
            case '7':
                return "seven";
            case '8':
                return "eight";
            case '9':
                return "nine";
            default:
                return std::nullopt;
            }
        }
    };
}
