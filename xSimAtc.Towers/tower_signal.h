#pragma once

#include <chrono>
#include <guiddef.h>
#include <string>

namespace xsim::towers
{
    struct TowerSignal final
    {
        GUID tower_id{};
        std::string location;
        std::chrono::steady_clock::time_point timestamp{};
    };
}
