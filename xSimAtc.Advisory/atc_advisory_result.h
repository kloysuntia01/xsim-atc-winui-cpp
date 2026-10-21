#pragma once

#include <string>

namespace xsim::advisory
{
    struct AtcAdvisoryResult final
    {
        std::string text;

        // Advisory services are read-only by contract. This flag exists
        // specifically so UI/integration smoke tests can surface violations.
        bool world_state_changed{ false };
    };
}
