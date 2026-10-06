#pragma once

#include "Chain/i_handler.h"
#include "Communication/communication_request.h"
#include "Engine/engine_request.h"
#include "States/aircraft_phase.h"
#include "States/state_request.h"
#include "aircraft_clocks.h"
#include "i_aircraft.h"
#include "i_aircraft_mediator.h"
#include "i_mediator.h"

#include <memory>
#include <string>

namespace xsim::aircrafts
{
    class AircraftBase :
        public IAircraft,
        public IMediator
    {
    public:
        AircraftBase();

        explicit AircraftBase(
            std::string call_sign);

        AircraftBase(
            std::string call_sign,
            std::shared_ptr<IAircraftMediator> mediator);

        [[nodiscard]]
        const GUID& id() const noexcept override;

        [[nodiscard]]
        const std::string& call_sign() const noexcept override;

        void tick() override;

        void send(CommunicationRequest request) override;
        void receive(const CommunicationRequest& request) override;

        void publish_transponder(
            const TransponderSignal& signal) override;

        [[nodiscard]]
        const EngineRequest& engine_state() const noexcept;

        [[nodiscard]]
        const CommunicationRequest& last_received() const noexcept;

        [[nodiscard]]
        AircraftPhase phase() const noexcept;

        [[nodiscard]]
        bool request_transition(AircraftPhase requested);

        [[nodiscard]]
        AircraftClocks& clocks() noexcept;

        [[nodiscard]]
        const AircraftClocks& clocks() const noexcept;

    protected:
        ~AircraftBase() override = default;

    private:
        void build_engine_chain();
        void build_communication_chain();
        void build_state_chain();

        GUID id_{};
        std::string call_sign_;
        std::shared_ptr<IAircraftMediator> mediator_;

        EngineRequest engine_state_;
        CommunicationRequest last_received_;
        AircraftPhase current_phase_{ AircraftPhase::Ready };
        AircraftClocks clocks_;

        std::shared_ptr<chain::IHandler<EngineRequest>> engine_chain_;
        std::shared_ptr<chain::IHandler<CommunicationRequest>> communication_chain_;
        std::shared_ptr<chain::IHandler<StateRequest>> state_chain_;
    };
}
