#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_movement_route_binding.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldMovementRouteBindingTests, AppliesExpandedTaxiwayRouteToMovement)
    {
        AirfieldMovement movement;

        const std::vector<std::string> selected{
            "F",
            "H",
            "J",
            "18L"
        };

        ASSERT_TRUE(
            movement_route_binding::apply_selected_route(
                movement,
                selected));

        EXPECT_FALSE(movement.is_running());
        EXPECT_FALSE(movement.is_completed());

        const auto expected_start =
            taxiway_layout::find_node("F");

        ASSERT_NE(expected_start, nullptr);

        EXPECT_DOUBLE_EQ(
            movement.position().x,
            expected_start->position.x);

        EXPECT_DOUBLE_EQ(
            movement.position().y,
            expected_start->position.y);
    }

    TEST(AirfieldMovementRouteBindingTests, ApplyAndStartBeginsMovement)
    {
        AirfieldMovement movement;

        const std::vector<std::string> selected{
            "F",
            "H"
        };

        ASSERT_TRUE(
            movement_route_binding::apply_and_start_selected_route(
                movement,
                selected));

        EXPECT_TRUE(movement.is_running());
        EXPECT_FALSE(movement.is_completed());
    }

    TEST(AirfieldMovementRouteBindingTests, InvalidRouteDoesNotStartMovement)
    {
        AirfieldMovement movement;

        const std::vector<std::string> selected{
            "F",
            "NOT-A-NODE"
        };

        EXPECT_FALSE(
            movement_route_binding::apply_and_start_selected_route(
                movement,
                selected));

        EXPECT_FALSE(movement.is_running());
    }
}
