#pragma once

#include "atc_command.h"

#include <optional>
#include <string_view>

namespace xsimatc::speech
{
    class AtcCommandParser final
    {
    public:
        [[nodiscard]]
        std::optional<AtcCommand> parse(
            std::string_view normalized_text) const;
    };
}
