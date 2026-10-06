#pragma once

#include "../Collections/collection.h"
#include "../Positions/airfield_position.h"

#include <string>
#include <utility>

namespace xsim::airfields
{
    struct AircraftMarker final
    {
        std::string call_sign;
        AirfieldPosition position{};

        AircraftMarker() = default;

        AircraftMarker(
            std::string aircraft_call_sign,
            AirfieldPosition aircraft_position)
            : call_sign(std::move(aircraft_call_sign)),
              position(aircraft_position)
        {
        }
    };

    using AircraftMarkers =
        Collection<AircraftMarker>;
}
