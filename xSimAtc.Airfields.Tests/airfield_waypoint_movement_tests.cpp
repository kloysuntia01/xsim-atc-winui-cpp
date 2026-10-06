#include "pch.h"
#include "Routing/airfield_waypoint.h"
#include "Routing/airfield_waypoint_route.h"
#include "Routing/airfield_movement.h"
#include "airfield_layout.h"
#include <array>
#include "../xSimAtc.Airfields/Routing/airfield_movement_completion_binding.h"
namespace xsim::airfields::tests
{
    TEST(AirfieldWaypointTests, MidpointIsCentered)
    {
        const auto p = midpoint({0.2,0.4}, {0.6,0.8});
        EXPECT_DOUBLE_EQ(p.x, 0.4);
        EXPECT_DOUBLE_EQ(p.y, 0.6);
    }

    TEST(AirfieldWaypointTests, RouteAddsCenterWaypointBetweenEachNodePair)
    {
        const std::array nodes{ layout::F, layout::H, layout::J, layout::Runway18L };
        const auto route = build_center_waypoint_route(nodes);
        ASSERT_EQ(route.size(), 7u);
        EXPECT_EQ(route[1].id, "F-H-CENTER");
        EXPECT_EQ(route[3].id, "H-J-CENTER");
        EXPECT_EQ(route[5].id, "J-18L-CENTER");
    }

    TEST(AirfieldMovementTests, StartsAtFirstWaypoint)
    {
        AirfieldMovement movement;
        movement.set_route({ {"A",{0,0}}, {"B",{1,0}} });
        EXPECT_DOUBLE_EQ(movement.position().x, 0.0);
        EXPECT_FALSE(movement.is_running());
    }

    TEST(AirfieldMovementTests, AdvancesHalfwayAcrossSegment)
    {
        AirfieldMovement movement;
        movement.set_route({ {"A",{0,0}}, {"B",{1,0}} });
        movement.start();
        EXPECT_TRUE(movement.advance(0.5));
        EXPECT_DOUBLE_EQ(movement.position().x, 0.5);
    }

    TEST(AirfieldMovementTests, CarriesProgressIntoNextSegment)
    {
        AirfieldMovement movement;
        movement.set_route({ {"A",{0,0}}, {"B",{1,0}}, {"C",{1,1}} });
        movement.start();
        EXPECT_TRUE(movement.advance(1.5));
        EXPECT_DOUBLE_EQ(movement.position().x, 1.0);
        EXPECT_DOUBLE_EQ(movement.position().y, 0.5);
    }

    TEST(AirfieldMovementTests, CompletesAtFinalWaypoint)
    {
        AirfieldMovement movement;
        movement.set_route({ {"A",{0,0}}, {"B",{1,0}} });
        movement.start();
        EXPECT_TRUE(movement.advance(1.0));
        EXPECT_TRUE(movement.is_completed());
        EXPECT_FALSE(movement.is_running());
        EXPECT_DOUBLE_EQ(movement.position().x, 1.0);
    }

TEST(AirfieldMovementTests, InvokesCompletionCallbackExactlyOnce)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    int completed_count = 0;

    movement.set_completed_callback(
        [&completed_count]()
        {
            ++completed_count;
        });

    movement.set_route(route);
    movement.start();

    movement.advance(1.0);
    movement.advance(1.0);

    EXPECT_TRUE(movement.is_completed());
    EXPECT_EQ(completed_count, 1);
}

TEST(AirfieldMovementTests, NewRouteRearmsCompletionCallback)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    int completed_count = 0;

    movement.set_completed_callback(
        [&completed_count]()
        {
            ++completed_count;
        });

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    EXPECT_EQ(completed_count, 2);
}

namespace
{
    enum class FakeAircraftPhase
    {
        Ready,
        InPosition
    };

    struct FakeAircraft final
    {
        FakeAircraftPhase phase{ FakeAircraftPhase::Ready };
        int transition_requests{};

        bool request_transition(FakeAircraftPhase requested)
        {
            ++transition_requests;
            phase = requested;
            return true;
        }
    };
}

TEST(AirfieldMovementTests, BindsCompletionToRequestedPhase)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    FakeAircraft aircraft;

    bind_movement_completion_to_phase(
        movement,
        aircraft,
        FakeAircraftPhase::InPosition);

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    EXPECT_TRUE(movement.is_completed());
    EXPECT_EQ(aircraft.phase, FakeAircraftPhase::InPosition);
    EXPECT_EQ(aircraft.transition_requests, 1);
}

TEST(AirfieldMovementTests, DoesNotRequestPhaseBeforeMovementCompletes)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    FakeAircraft aircraft;

    bind_movement_completion_to_phase(
        movement,
        aircraft,
        FakeAircraftPhase::InPosition);

    movement.set_route(route);
    movement.start();
    movement.advance(0.5);

    EXPECT_FALSE(movement.is_completed());
    EXPECT_EQ(aircraft.phase, FakeAircraftPhase::Ready);
    EXPECT_EQ(aircraft.transition_requests, 0);
}
}



