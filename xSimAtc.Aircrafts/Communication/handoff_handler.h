#pragma once

#include "../Chain/handler_base.h"
#include "communication_request.h"

namespace xsim::aircrafts
{
    class HandoffHandler final :
        public chain::HandlerBase<CommunicationRequest>
    {
    public:
        void handle(CommunicationRequest& request) override
        {
            if (request.state == CommunicationState::HandingOff &&
                !guid_is_empty(request.handoff_guid))
            {
                request.current_peer_guid = request.handoff_guid;
                request.receiver_guid = request.handoff_guid;
                request.handoff_guid = {};
                request.state = CommunicationState::Active;
            }

            next(request);
        }
    };
}
