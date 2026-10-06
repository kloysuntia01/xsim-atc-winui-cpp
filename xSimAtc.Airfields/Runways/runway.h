#pragma once

#include "../Collections/collection.h"
#include "../Positions/airfield_position.h"

#include <string>
#include <utility>

namespace xsim::airfields
{
    struct Runway final
    {
        std::string id;
        AirfieldPosition north_end{};
        AirfieldPosition south_end{};

        Runway() = default;

        Runway(
            std::string runway_id,
            AirfieldPosition north,
            AirfieldPosition south)
            : id(std::move(runway_id)),
              north_end(north),
              south_end(south)
        {
        }
    };

    using Runways =
        Collection<Runway>;
}
