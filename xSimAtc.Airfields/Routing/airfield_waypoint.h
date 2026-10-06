#pragma once
#include "../Positions/airfield_position.h"
#include <string>
#include <utility>

namespace xsim::airfields
{
    struct AirfieldWaypoint final
    {
        std::string id;
        AirfieldPosition position;
        constexpr bool operator==(const AirfieldWaypoint&) const noexcept = default;
    };

    [[nodiscard]] constexpr AirfieldPosition midpoint(AirfieldPosition from, AirfieldPosition to) noexcept
    {
        return { (from.x + to.x) / 2.0, (from.y + to.y) / 2.0 };
    }

    [[nodiscard]] inline AirfieldWaypoint make_center_waypoint(
        std::string id, AirfieldPosition from, AirfieldPosition to)
    {
        return { std::move(id), midpoint(from, to) };
    }
}
