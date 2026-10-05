#pragma once

#include "../Chain/handler_base.h"
#include "engine_request.h"

namespace xsim::aircrafts
{
    class PositionHandler final :
        public chain::HandlerBase<EngineRequest>
    {
    public:
        void handle(EngineRequest& request) override
        {
            if (request.engine_running)
            {
                request.x += request.velocity_x * request.speed;
                request.y += request.velocity_y * request.speed;
            }

            next(request);
        }
    };
}
