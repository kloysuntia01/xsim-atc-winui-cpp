#pragma once

#include "airfield_taxiway_waypoint_route.h"
#include "Routing/airfield_movement.h"

#include <string>
#include <vector>

namespace xsim::airfields::movement_route_binding
{
    inline bool apply_selected_route(
        AirfieldMovement& movement,
        const std::vector<std::string>& selected_nodes)
    {
        auto waypoints =
            taxiway_waypoint_route::build(
                selected_nodes);

        if (waypoints.size() < 2)
        {
            movement.stop();
            return false;
        }

        movement.set_route(
            std::move(waypoints));

        return true;
    }

    inline bool apply_and_start_selected_route(
        AirfieldMovement& movement,
        const std::vector<std::string>& selected_nodes)
    {
        if (!apply_selected_route(
                movement,
                selected_nodes))
        {
            return false;
        }

        movement.start();
        return true;
    }
}

