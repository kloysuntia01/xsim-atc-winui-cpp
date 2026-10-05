#pragma once

#include <cstdint>
#include <guiddef.h>
#include <string>

namespace xsim::communications
{
    struct TrackingState final
    {
        GUID aircraft_id{};
        std::string call_sign;
        std::uint64_t sequence{};
        double x{};
        double y{};
    };
}
