#pragma once

#include <memory>

#include "../Chain/i_handler.h"
#include "completed_handler.h"
#include "holding_handler.h"
#include "in_position_handler.h"
#include "incoming_handler.h"
#include "landing_handler.h"
#include "outbound_handler.h"
#include "ready_handler.h"
#include "taking_off_handler.h"
#include "taxi_handler.h"

namespace xsim::aircrafts
{
    [[nodiscard]]
    inline std::shared_ptr<chain::IHandler<StateRequest>> make_state_chain()
    {
        auto ready = std::make_shared<ReadyHandler>();
        auto taxi = std::make_shared<TaxiHandler>();
        auto in_position = std::make_shared<InPositionHandler>();
        auto taking_off = std::make_shared<TakingOffHandler>();
        auto outbound = std::make_shared<OutboundHandler>();
        auto incoming = std::make_shared<IncomingHandler>();
        auto holding = std::make_shared<HoldingHandler>();
        auto landing = std::make_shared<LandingHandler>();
        auto completed = std::make_shared<CompletedHandler>();

        ready->set_next(taxi);
        taxi->set_next(in_position);
        in_position->set_next(taking_off);
        taking_off->set_next(outbound);
        outbound->set_next(incoming);
        incoming->set_next(holding);
        holding->set_next(landing);
        landing->set_next(completed);

        return ready;
    }
}
