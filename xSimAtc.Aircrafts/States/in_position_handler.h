#pragma once

#include "../Chain/handler_base.h"
#include "state_request.h"

namespace xsim::aircrafts
{
    class InPositionHandler final : public chain::HandlerBase<StateRequest>
    {
    public:
        void handle(StateRequest& request) override
        {
            if (request.current == AircraftPhase::InPosition &&
                request.requested == AircraftPhase::TakingOff)
            {
                request.handled = true;
                request.allowed = true;
                return;
            }

            next(request);
        }
    };
}
