#pragma once

#include "Communication/communication_request.h"

namespace xsim::aircrafts
{
    struct TransponderSignal;

    // Participant-facing mediator role. AircraftBase implements this role;
    // routing still goes through IAircraftMediator -> Communications -> Msg.Q.
    class IMediator
    {
    public:
        virtual ~IMediator() = default;

        virtual void send(CommunicationRequest request) = 0;
        virtual void receive(const CommunicationRequest& request) = 0;

        virtual void publish_transponder(
            const TransponderSignal& signal) = 0;
    };
}
