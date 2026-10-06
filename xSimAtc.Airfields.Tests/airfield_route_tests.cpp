#include "pch.h"

#include "Routing/airfield_route.h"
#include "airfield_layout.h"

namespace xsim::airfields::tests
{
    namespace
    {
        constexpr double epsilon = 0.000001;
    }

    TEST(AirfieldRouteTests, RouteStartsAtF)
    {
        const auto position =
            position_along_route(
                layout::TaxiF_H_J_18L,
                0.0);

        EXPECT_NEAR(
            position.x,
            layout::F.position.x,
            epsilon);

        EXPECT_NEAR(
            position.y,
            layout::F.position.y,
            epsilon);
    }

    TEST(AirfieldRouteTests, RouteEndsAtRunway18L)
    {
        const auto position =
            position_along_route(
                layout::TaxiF_H_J_18L,
                1.0);

        EXPECT_NEAR(
            position.x,
            layout::Runway18L.position.x,
            epsilon);

        EXPECT_NEAR(
            position.y,
            layout::Runway18L.position.y,
            epsilon);
    }

    TEST(AirfieldRouteTests, HalfwayInterpolatesBetweenHAndJ)
    {
        const auto position =
            position_along_route(
                layout::TaxiF_H_J_18L,
                0.5);

        const auto expected_x =
            (layout::H.position.x +
             layout::J.position.x) / 2.0;

        const auto expected_y =
            (layout::H.position.y +
             layout::J.position.y) / 2.0;

        EXPECT_NEAR(position.x, expected_x, epsilon);
        EXPECT_NEAR(position.y, expected_y, epsilon);
    }

    TEST(AirfieldRouteTests, ProgressIsClampedToRouteEnds)
    {
        const auto before_start =
            position_along_route(
                layout::TaxiF_H_J_18L,
                -1.0);

        const auto after_end =
            position_along_route(
                layout::TaxiF_H_J_18L,
                2.0);

        EXPECT_EQ(
            before_start,
            layout::F.position);

        EXPECT_EQ(
            after_end,
            layout::Runway18L.position);
    }
}
