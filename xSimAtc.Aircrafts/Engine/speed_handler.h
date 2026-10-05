#pragma once

#include "../Chain/handler_base.h"
#include "engine_request.h"

namespace xsim::aircrafts
{
    class SpeedHandler final :
        public chain::HandlerBase<EngineRequest>
    {
    public:
        void handle(EngineRequest& request) override
        {
            request.speed =
                request.engine_running
                ? request.throttle * 5.0
                : 0.0;

            next(request);
        }
    };
}
