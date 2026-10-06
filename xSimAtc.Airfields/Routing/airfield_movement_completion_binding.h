#pragma once

#include "airfield_movement.h"

namespace xsim::airfields
{
    template<typename TAircraft, typename TPhase>
    void bind_movement_completion_to_phase(
        AirfieldMovement& movement,
        TAircraft& aircraft,
        TPhase phase)
    {
        movement.set_completed_callback(
            [&aircraft, phase]()
            {
                aircraft.request_transition(phase);
            });
    }
}
