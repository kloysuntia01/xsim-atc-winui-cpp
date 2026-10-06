#include "pch.h"
#include "speech_text_normalizer.h"

#include <cctype>

namespace xsimatc::speech
{
    std::string SpeechTextNormalizer::normalize(const std::string_view text)
    {
        std::string normalized;
        normalized.reserve(text.size());

        bool pending_space = false;

        for (const unsigned char ch : text)
        {
            if (std::isalnum(ch) != 0)
            {
                if (pending_space && !normalized.empty())
                {
                    normalized.push_back(' ');
                }

                normalized.push_back(
                    static_cast<char>(std::tolower(ch)));
                pending_space = false;
                continue;
            }

            // Treat whitespace and punctuation as a word boundary.
            pending_space = !normalized.empty();
        }

        return normalized;
    }
}
