#pragma once

#include "aircraft_phase.h"

namespace xsim::aircrafts
{
    struct StateRequest final
    {
        AircraftPhase current{ AircraftPhase::Ready };
        AircraftPhase requested{ AircraftPhase::Ready };
        bool handled{ false };
        bool allowed{ false };
    };
}
