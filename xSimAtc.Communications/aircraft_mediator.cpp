#include "pch.h"
#include "aircraft_mediator.h"

#include "Communication/communication_request.h"
#include "transponder_signal.h"

#include <utility>

namespace xsim::communications
{
    AircraftMediator::AircraftMediator(
        std::shared_ptr<MessageQueue> message_queue)
        : message_queue_(std::move(message_queue))
    {
    }

    void AircraftMediator::publish_transponder(
        const xsim::aircrafts::TransponderSignal& signal)
    {
        message_queue_->register_tracking(
            TrackingState{
                signal.aircraft_id,
                signal.call_sign,
                signal.sequence,
                signal.x,
                signal.y
            });
    }

    void AircraftMediator::publish_communication(
        const xsim::aircrafts::CommunicationRequest& request)
    {
        message_queue_->register_communication(request);
    }
}
