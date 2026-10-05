#pragma once

#include <chrono>
#include <cstdint>
#include <guiddef.h>
#include <string>

namespace xsim::aircrafts
{
    struct TransponderSignal final
    {
        GUID aircraft_id{};
        std::string call_sign;
        std::uint64_t sequence{};
        double x{};
        double y{};
        std::chrono::steady_clock::time_point timestamp{};
    };
}
