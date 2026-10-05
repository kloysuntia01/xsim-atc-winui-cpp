#pragma once

#include "../Chain/handler_base.h"
#include "communication_request.h"

namespace xsim::aircrafts
{
    class HailingHandler final :
        public chain::HandlerBase<CommunicationRequest>
    {
    public:
        void handle(CommunicationRequest& request) override
        {
            if (request.state == CommunicationState::Hailing &&
                guid_is_empty(request.originator_guid))
            {
                request.originator_guid = request.sender_guid;
            }

            next(request);
        }
    };
}
