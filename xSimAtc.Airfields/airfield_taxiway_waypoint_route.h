#pragma once

#include "airfield_taxiway_layout.h"
#include "Routing/airfield_waypoint.h"

#include <string>
#include <vector>

namespace xsim::airfields::taxiway_waypoint_route
{
    inline std::vector<AirfieldWaypoint> build(
        const std::vector<std::string>& control_nodes)
    {
        const auto graph =
            taxiway_layout::build_graph();

        const auto expanded =
            taxiway_layout::expand_control_route(
                graph,
                control_nodes);

        if (expanded.empty())
        {
            return {};
        }

        std::vector<AirfieldWaypoint> result;
        result.reserve(expanded.size());

        for (const auto& node_id : expanded)
        {
            const auto* node =
                taxiway_layout::find_node(node_id);

            if (node == nullptr)
            {
                return {};
            }

            result.push_back(
                AirfieldWaypoint{
                    std::string{ node->id },
                    node->position
                });
        }

        return result;
    }
}

