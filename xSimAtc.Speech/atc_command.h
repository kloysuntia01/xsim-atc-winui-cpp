#pragma once

#include "atc_action.h"

#include <optional>
#include <string>

namespace xsimatc::speech
{
    struct AtcCommand final
    {
        std::string call_sign;
        AtcAction action{ AtcAction::Unknown };
        std::optional<std::string> runway;
    };
}
