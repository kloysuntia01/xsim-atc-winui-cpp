#include "pch.h"

#include "aircraft.h"

#include <gtest/gtest.h>

namespace xsim::aircrafts::tests
{
    TEST(AircraftTests, ConstructorPreservesCallSign)
    {
        Aircraft aircraft{ "FDX606" };

        EXPECT_EQ(
            aircraft.call_sign(),
            "FDX606");
    }

    TEST(AircraftTests, ConstructorGeneratesNonEmptyGuid)
    {
        Aircraft aircraft{ "FDX606" };

        const GUID empty_guid{};

        EXPECT_NE(
            std::memcmp(
                &aircraft.id(),
                &empty_guid,
                sizeof(GUID)),
            0);
    }

    TEST(AircraftTests, TwoAircraftGenerateDifferentGuids)
    {
        Aircraft first{ "FDX606" };
        Aircraft second{ "DAL101" };

        EXPECT_NE(
            std::memcmp(
                &first.id(),
                &second.id(),
                sizeof(GUID)),
            0);
    }


    TEST(AircraftTests, AircraftStartsReady)
    {
        Aircraft aircraft{ "FDX606" };

        EXPECT_EQ(
            aircraft.phase(),
            AircraftPhase::Ready);
    }

    TEST(AircraftTests, AircraftCanTransitionReadyToTaxi)
    {
        Aircraft aircraft{ "FDX606" };

        const auto transitioned = aircraft.request_transition(
            AircraftPhase::Taxi);

        EXPECT_TRUE(transitioned);
        EXPECT_EQ(
            aircraft.phase(),
            AircraftPhase::Taxi);
    }

    TEST(AircraftTests, AircraftRejectsReadyToLanding)
    {
        Aircraft aircraft{ "FDX606" };

        const auto transitioned = aircraft.request_transition(
            AircraftPhase::Landing);

        EXPECT_FALSE(transitioned);
    }

    TEST(AircraftTests, AircraftPhaseChangesOnlyWhenStateChainAllowsIt)
    {
        Aircraft aircraft{ "FDX606" };

        EXPECT_FALSE(aircraft.request_transition(
            AircraftPhase::Landing));
        EXPECT_EQ(
            aircraft.phase(),
            AircraftPhase::Ready);

        EXPECT_TRUE(aircraft.request_transition(
            AircraftPhase::Taxi));
        EXPECT_EQ(
            aircraft.phase(),
            AircraftPhase::Taxi);
    }


    TEST(AircraftTests, AircraftStartsWithNoClocks)
    {
        Aircraft aircraft{ "FDX606" };

        EXPECT_TRUE(aircraft.clocks().empty());
        EXPECT_EQ(aircraft.clocks().size(), 0u);
    }

}
