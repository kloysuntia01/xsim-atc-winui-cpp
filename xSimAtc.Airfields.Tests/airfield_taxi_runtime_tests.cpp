#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxi_runtime.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiRuntimeTests, LoadsSelectedRouteIntoMovement)
    {
        AirfieldTaxiRuntime runtime;

        ASSERT_TRUE(
            runtime.load_route({
                "F",
                "H",
                "J",
                "18L"
            }));

        EXPECT_FALSE(runtime.is_running());
        EXPECT_FALSE(runtime.is_completed());

        const auto* start =
            taxiway_layout::find_node("F");

        ASSERT_NE(start, nullptr);

        EXPECT_DOUBLE_EQ(
            runtime.position().x,
            start->position.x);

        EXPECT_DOUBLE_EQ(
            runtime.position().y,
            start->position.y);
    }

    TEST(AirfieldTaxiRuntimeTests, LoadAndStartBeginsTaxi)
    {
        AirfieldTaxiRuntime runtime;

        ASSERT_TRUE(
            runtime.load_and_start({
                "F",
                "H"
            }));

        EXPECT_TRUE(runtime.is_running());
        EXPECT_FALSE(runtime.is_completed());
    }

    TEST(AirfieldTaxiRuntimeTests, AdvanceMovesAircraftAlongTaxiwayRoute)
    {
        AirfieldTaxiRuntime runtime;

        ASSERT_TRUE(
            runtime.load_and_start({
                "F",
                "H"
            }));

        const auto before =
            runtime.position();

        runtime.advance(0.5);

        const auto after =
            runtime.position();

        EXPECT_TRUE(
            before.x != after.x ||
            before.y != after.y);
    }

    TEST(AirfieldTaxiRuntimeTests, CompletingRouteStopsRuntimeAtDestination)
    {
        AirfieldTaxiRuntime runtime;

        ASSERT_TRUE(
            runtime.load_and_start({
                "F",
                "H"
            }));

        runtime.advance(10.0);

        EXPECT_TRUE(runtime.is_completed());
        EXPECT_FALSE(runtime.is_running());

        const auto* destination =
            taxiway_layout::find_node("H");

        ASSERT_NE(destination, nullptr);

        EXPECT_DOUBLE_EQ(
            runtime.position().x,
            destination->position.x);

        EXPECT_DOUBLE_EQ(
            runtime.position().y,
            destination->position.y);
    }
}
