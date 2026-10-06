#pragma once

#include "../Positions/airfield_position.h"

#include <algorithm>

namespace xsim::airfields
{
    [[nodiscard]]
    constexpr AirfieldPosition interpolate(
        AirfieldPosition from,
        AirfieldPosition to,
        double progress) noexcept
    {
        const auto t =
            std::clamp(progress, 0.0, 1.0);

        return
        {
            from.x + ((to.x - from.x) * t),
            from.y + ((to.y - from.y) * t)
        };
    }
}
