#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_waypoint_route.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiwayWaypointRouteTests, BuildsMovementWaypointsFromControllerRoute)
    {
        const std::vector<std::string> selected{
            "F",
            "H",
            "J",
            "18L"
        };

        const auto waypoints =
            taxiway_waypoint_route::build(
                selected);

        ASSERT_EQ(waypoints.size(), 7u);

        EXPECT_EQ(waypoints[0].id, "F");
        EXPECT_EQ(waypoints[1].id, "F1");
        EXPECT_EQ(waypoints[2].id, "H");
        EXPECT_EQ(waypoints[3].id, "H1");
        EXPECT_EQ(waypoints[4].id, "J");
        EXPECT_EQ(waypoints[5].id, "J1");
        EXPECT_EQ(waypoints[6].id, "18L");
    }

    TEST(AirfieldTaxiwayWaypointRouteTests, WaypointsUseTaxiwayLayoutPositions)
    {
        const std::vector<std::string> selected{
            "F",
            "H"
        };

        const auto waypoints =
            taxiway_waypoint_route::build(
                selected);

        ASSERT_EQ(waypoints.size(), 3u);

        const auto* f1 =
            taxiway_layout::find_node("F1");

        ASSERT_NE(f1, nullptr);

        EXPECT_DOUBLE_EQ(
            waypoints[1].position.x,
            f1->position.x);

        EXPECT_DOUBLE_EQ(
            waypoints[1].position.y,
            f1->position.y);
    }

    TEST(AirfieldTaxiwayWaypointRouteTests, InvalidControllerRouteProducesNoWaypoints)
    {
        const std::vector<std::string> selected{
            "F",
            "NOT-A-NODE"
        };

        EXPECT_TRUE(
            taxiway_waypoint_route::build(
                selected).empty());
    }
}
