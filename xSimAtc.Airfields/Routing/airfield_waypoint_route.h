#pragma once
#include "airfield_waypoint.h"
#include "../Nodes/airfield_node.h"
#include <span>
#include <vector>

namespace xsim::airfields
{
    [[nodiscard]] inline std::vector<AirfieldWaypoint> build_center_waypoint_route(
        std::span<const AirfieldNode> nodes)
    {
        std::vector<AirfieldWaypoint> waypoints;
        if (nodes.empty()) return waypoints;
        waypoints.reserve((nodes.size() * 2u) - 1u);
        waypoints.push_back({ nodes.front().id, nodes.front().position });
        for (std::size_t index = 1; index < nodes.size(); ++index)
        {
            const auto& from = nodes[index - 1];
            const auto& to = nodes[index];
            waypoints.push_back(make_center_waypoint(
                from.id + "-" + to.id + "-CENTER", from.position, to.position));
            waypoints.push_back({ to.id, to.position });
        }
        return waypoints;
    }
}
