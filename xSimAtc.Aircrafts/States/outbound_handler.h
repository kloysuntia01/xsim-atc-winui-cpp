#pragma once

#include "../Chain/handler_base.h"
#include "state_request.h"

namespace xsim::aircrafts
{
    class OutboundHandler final : public chain::HandlerBase<StateRequest>
    {
    public:
        void handle(StateRequest& request) override
        {
            if (request.current == AircraftPhase::Outbound &&
                request.requested == AircraftPhase::Incoming)
            {
                request.handled = true;
                request.allowed = true;
                return;
            }

            next(request);
        }
    };
}
