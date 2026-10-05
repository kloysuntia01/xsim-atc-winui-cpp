#pragma once

namespace xsim::aircrafts
{
    struct CommunicationRequest;
    struct TransponderSignal;

    // Routing boundary implemented by xSimAtc.Communications.
    class IAircraftMediator
    {
    public:
        virtual ~IAircraftMediator() = default;

        virtual void publish_transponder(
            const TransponderSignal& signal) = 0;

        virtual void publish_communication(
            const CommunicationRequest& request) = 0;
    };
}
