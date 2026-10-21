#pragma once

#include <cctype>
#include <string>
#include <string_view>

namespace xsim::inference
{
    [[nodiscard]]
    inline std::string normalize_inference_text(
        const std::string_view raw_text)
    {
        constexpr std::string_view think_open = "<think>";
        constexpr std::string_view think_close = "</think>";

        auto first = std::size_t{ 0 };

        while (first < raw_text.size() &&
               std::isspace(
                   static_cast<unsigned char>(
                       raw_text[first])) != 0)
        {
            ++first;
        }

        // Only normalize a leading reasoning block. Never remove content
        // from the middle of an otherwise ordinary model response.
        if (!raw_text.substr(first).starts_with(think_open))
        {
            return std::string(raw_text);
        }

        const auto close = raw_text.find(
            think_close,
            first + think_open.size());

        // Incomplete reasoning output is preserved verbatim so callers do
        // not lose diagnostic/model text.
        if (close == std::string_view::npos)
        {
            return std::string(raw_text);
        }

        auto content_start =
            close + think_close.size();

        while (content_start < raw_text.size() &&
               std::isspace(
                   static_cast<unsigned char>(
                       raw_text[content_start])) != 0)
        {
            ++content_start;
        }

        return std::string(
            raw_text.substr(content_start));
    }
}
