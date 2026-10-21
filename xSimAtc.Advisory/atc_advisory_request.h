#pragma once

#include <string>

namespace xsim::advisory
{
    struct AtcAdvisoryRequest final
    {
        std::string runway;
        std::string runway_status;
        std::string call_sign;
        std::string aircraft_status;
    };
}
