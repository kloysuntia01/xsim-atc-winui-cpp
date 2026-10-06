#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_layout.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldOptionBWaypointCalibrationTests, F1SitsOnUpperTaxiwayCenterline)
    {
        const auto* f1 = taxiway_layout::find_node("F1");

        ASSERT_NE(f1, nullptr);
        EXPECT_DOUBLE_EQ(f1->position.x, 0.595);
        EXPECT_DOUBLE_EQ(f1->position.y, 0.329);
    }

    TEST(AirfieldOptionBWaypointCalibrationTests, H1SitsOnLowerTaxiwayCenterline)
    {
        const auto* h1 = taxiway_layout::find_node("H1");

        ASSERT_NE(h1, nullptr);
        EXPECT_DOUBLE_EQ(h1->position.x, 0.470);
        EXPECT_DOUBLE_EQ(h1->position.y, 0.729);
    }

    TEST(AirfieldOptionBWaypointCalibrationTests, J1SitsOnQVerticalTaxiway)
    {
        const auto* j1 = taxiway_layout::find_node("J1");

        ASSERT_NE(j1, nullptr);
        EXPECT_DOUBLE_EQ(j1->position.x, 0.400);
        EXPECT_DOUBLE_EQ(j1->position.y, 0.835);
    }
}
