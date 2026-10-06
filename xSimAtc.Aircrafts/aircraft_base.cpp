#include "pch.h"
#include "aircraft_base.h"

#include "Communication/active_handler.h"
#include "Communication/completion_handler.h"
#include "Communication/hailing_handler.h"
#include "Communication/handoff_handler.h"
#include "Communication/pairing_handler.h"
#include "Engine/engine_state_handler.h"
#include "Engine/fuel_handler.h"
#include "Engine/position_handler.h"
#include "Engine/speed_handler.h"
#include "Engine/throttle_handler.h"
#include "States/state_chain.h"
#include "transponder_signal.h"

#include <objbase.h>
#include <stdexcept>
#include <utility>

namespace xsim::aircrafts
{
    AircraftBase::AircraftBase()
        : AircraftBase("")
    {
    }

    AircraftBase::AircraftBase(
        std::string call_sign)
        : AircraftBase(
            std::move(call_sign),
            nullptr)
    {
    }

    AircraftBase::AircraftBase(
        std::string call_sign,
        std::shared_ptr<IAircraftMediator> mediator)
        : call_sign_(std::move(call_sign)),
          mediator_(std::move(mediator))
    {
        const auto result = ::CoCreateGuid(&id_);

        if (FAILED(result))
        {
            throw std::runtime_error(
                "Unable to generate aircraft GUID.");
        }

        build_engine_chain();
        build_communication_chain();
        build_state_chain();
    }

    const GUID& AircraftBase::id() const noexcept
    {
        return id_;
    }

    const std::string& AircraftBase::call_sign() const noexcept
    {
        return call_sign_;
    }

    void AircraftBase::tick()
    {
        if (engine_chain_)
        {
            engine_chain_->handle(engine_state_);
        }
    }

    void AircraftBase::send(
        CommunicationRequest request)
    {
        if (guid_is_empty(request.sender_guid))
        {
            request.sender_guid = id_;
        }

        if (guid_is_empty(request.originator_guid))
        {
            request.originator_guid = id_;
        }

        if (communication_chain_)
        {
            communication_chain_->handle(request);
        }

        if (mediator_)
        {
            mediator_->publish_communication(request);
        }
    }

    void AircraftBase::receive(
        const CommunicationRequest& request)
    {
        last_received_ = request;
    }

    void AircraftBase::publish_transponder(
        const TransponderSignal& signal)
    {
        if (mediator_)
        {
            mediator_->publish_transponder(signal);
        }
    }

    const EngineRequest&
        AircraftBase::engine_state() const noexcept
    {
        return engine_state_;
    }

    const CommunicationRequest&
        AircraftBase::last_received() const noexcept
    {
        return last_received_;
    }

    AircraftPhase AircraftBase::phase() const noexcept
    {
        return current_phase_;
    }

    bool AircraftBase::request_transition(
        AircraftPhase requested)
    {
        if (!state_chain_)
        {
            return false;
        }

        StateRequest request{ current_phase_, requested };
        state_chain_->handle(request);

        if (!request.allowed)
        {
            return false;
        }

        current_phase_ = requested;
        return true;
    }

    AircraftClocks& AircraftBase::clocks() noexcept
    {
        return clocks_;
    }

    const AircraftClocks& AircraftBase::clocks() const noexcept
    {
        return clocks_;
    }


    void AircraftBase::build_engine_chain()
    {
        auto throttle = std::make_shared<ThrottleHandler>();
        auto fuel = std::make_shared<FuelHandler>();
        auto engine = std::make_shared<EngineStateHandler>();
        auto speed = std::make_shared<SpeedHandler>();
        auto position = std::make_shared<PositionHandler>();

        throttle->set_next(fuel);
        fuel->set_next(engine);
        engine->set_next(speed);
        speed->set_next(position);

        engine_chain_ = std::move(throttle);
    }

    void AircraftBase::build_state_chain()
    {
        state_chain_ = make_state_chain();
    }

    void AircraftBase::build_communication_chain()
    {
        auto hailing = std::make_shared<HailingHandler>();
        auto pairing = std::make_shared<PairingHandler>();
        auto active = std::make_shared<ActiveHandler>();
        auto handoff = std::make_shared<HandoffHandler>();
        auto completion = std::make_shared<CompletionHandler>();

        hailing->set_next(pairing);
        pairing->set_next(active);
        active->set_next(handoff);
        handoff->set_next(completion);

        communication_chain_ = std::move(hailing);
    }
}
