#pragma once

#include "../Collections/collection.h"
#include "../Positions/airfield_position.h"

#include <string>
#include <utility>

namespace xsim::airfields
{
    struct TowerMarker final
    {
        std::string id;
        AirfieldPosition position{};

        TowerMarker() = default;

        TowerMarker(
            std::string tower_id,
            AirfieldPosition tower_position)
            : id(std::move(tower_id)),
              position(tower_position)
        {
        }
    };

    using TowerMarkers =
        Collection<TowerMarker>;
}
