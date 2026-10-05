#pragma once

#include "../Chain/handler_base.h"
#include "engine_request.h"

namespace xsim::aircrafts
{
    class EngineStateHandler final :
        public chain::HandlerBase<EngineRequest>
    {
    public:
        void handle(EngineRequest& request) override
        {
            request.engine_running = request.fuel > 0.0;
            next(request);
        }
    };
}
