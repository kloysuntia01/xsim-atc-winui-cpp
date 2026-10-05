#pragma once

#include "../Chain/handler_base.h"
#include "engine_request.h"

#include <algorithm>

namespace xsim::aircrafts
{
    class FuelHandler final :
        public chain::HandlerBase<EngineRequest>
    {
    public:
        void handle(EngineRequest& request) override
        {
            if (request.fuel <= 0.0)
            {
                request.fuel = 0.0;
                request.engine_running = false;
                return;
            }

            request.fuel = (std::max)(
                0.0,
                request.fuel - (request.throttle * 0.01));

            next(request);
        }
    };
}
