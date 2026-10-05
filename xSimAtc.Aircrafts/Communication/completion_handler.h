#pragma once

#include "../Chain/handler_base.h"
#include "communication_request.h"

namespace xsim::aircrafts
{
    class CompletionHandler final :
        public chain::HandlerBase<CommunicationRequest>
    {
    public:
        void handle(CommunicationRequest& request) override
        {
            if (request.state == CommunicationState::Completed)
            {
                return;
            }

            next(request);
        }
    };
}
