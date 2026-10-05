#pragma once

#include "../Chain/handler_base.h"
#include "engine_request.h"

#include <algorithm>

namespace xsim::aircrafts
{
    class ThrottleHandler final :
        public chain::HandlerBase<EngineRequest>
    {
    public:
        void handle(EngineRequest& request) override
        {
            request.throttle =
                std::clamp(request.throttle, 0.0, 1.0);

            next(request);
        }
    };
}
