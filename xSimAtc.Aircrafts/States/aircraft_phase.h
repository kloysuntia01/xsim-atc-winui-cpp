#pragma once

namespace xsim::aircrafts
{
    enum class AircraftPhase
    {
        Ready,
        Taxi,
        InPosition,
        TakingOff,
        Outbound,
        Incoming,
        Holding,
        Landing,
        Completed
    };
}
