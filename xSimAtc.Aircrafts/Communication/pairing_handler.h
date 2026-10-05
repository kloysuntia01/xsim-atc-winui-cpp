#pragma once

#include "../Chain/handler_base.h"
#include "communication_request.h"

namespace xsim::aircrafts
{
    class PairingHandler final :
        public chain::HandlerBase<CommunicationRequest>
    {
    public:
        void handle(CommunicationRequest& request) override
        {
            if (request.state == CommunicationState::Hailing &&
                !guid_is_empty(request.receiver_guid))
            {
                request.current_peer_guid = request.receiver_guid;
                request.state = CommunicationState::Active;
            }

            next(request);
        }
    };
}
