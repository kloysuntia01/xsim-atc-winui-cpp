#pragma once

#include "i_aircraft_mediator.h"
#include "message_queue.h"

#include <memory>

namespace xsim::communications
{
    class AircraftMediator final :
        public xsim::aircrafts::IAircraftMediator
    {
    public:
        explicit AircraftMediator(
            std::shared_ptr<MessageQueue> message_queue);

        void publish_transponder(
            const xsim::aircrafts::TransponderSignal& signal) override;

        void publish_communication(
            const xsim::aircrafts::CommunicationRequest& request) override;

    private:
        std::shared_ptr<MessageQueue> message_queue_;
    };
}
