#pragma once

#include "../Nodes/airfield_node.h"
#include "airfield_segment.h"

#include <algorithm>
#include <cstddef>
#include <span>

namespace xsim::airfields
{
    [[nodiscard]]
    constexpr AirfieldPosition position_along_route(
        std::span<const AirfieldNode> nodes,
        double progress) noexcept
    {
        if (nodes.empty())
        {
            return {};
        }

        if (nodes.size() == 1)
        {
            return nodes.front().position;
        }

        const auto route_progress =
            std::clamp(progress, 0.0, 1.0);

        if (route_progress >= 1.0)
        {
            return nodes.back().position;
        }

        const auto segment_count =
            nodes.size() - 1;

        const auto scaled =
            route_progress *
            static_cast<double>(segment_count);

        const auto segment_index =
            (std::min)(
                static_cast<std::size_t>(scaled),
                segment_count - 1);

        const auto local_progress =
            scaled -
            static_cast<double>(segment_index);

        return interpolate(
            nodes[segment_index].position,
            nodes[segment_index + 1].position,
            local_progress);
    }
}

