#include "pch.h"
#include <gtest/gtest.h>

#include "../xSimAtc.Aircrafts/States/state_chain.h"

namespace xsim::aircrafts::tests
{
    TEST(StateChainTests, ReadyCanTransitionToTaxi)
    {
        auto chain = make_state_chain();
        StateRequest request{ AircraftPhase::Ready, AircraftPhase::Taxi };

        chain->handle(request);

        EXPECT_TRUE(request.handled);
        EXPECT_TRUE(request.allowed);
    }

    TEST(StateChainTests, HoldingCanTransitionToLanding)
    {
        auto chain = make_state_chain();
        StateRequest request{ AircraftPhase::Holding, AircraftPhase::Landing };

        chain->handle(request);

        EXPECT_TRUE(request.handled);
        EXPECT_TRUE(request.allowed);
    }

    TEST(StateChainTests, InvalidTransitionIsNotHandled)
    {
        auto chain = make_state_chain();
        StateRequest request{ AircraftPhase::Ready, AircraftPhase::Landing };

        chain->handle(request);

        EXPECT_FALSE(request.handled);
        EXPECT_FALSE(request.allowed);
    }

    TEST(StateChainTests, CompletedCanResetToReady)
    {
        auto chain = make_state_chain();
        StateRequest request{ AircraftPhase::Completed, AircraftPhase::Ready };

        chain->handle(request);

        EXPECT_TRUE(request.handled);
        EXPECT_TRUE(request.allowed);
    }
}
