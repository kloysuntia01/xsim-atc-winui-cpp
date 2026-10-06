#pragma once

#include "../Chain/handler_base.h"
#include "state_request.h"

namespace xsim::aircrafts
{
    class LandingHandler final : public chain::HandlerBase<StateRequest>
    {
    public:
        void handle(StateRequest& request) override
        {
            if (request.current == AircraftPhase::Landing &&
                request.requested == AircraftPhase::Completed)
            {
                request.handled = true;
                request.allowed = true;
                return;
            }

            next(request);
        }
    };
}
