#include "pch.h"

#include "Routing/airfield_route_selection.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldRouteSelectionTests, StartsEmpty)
    {
        AirfieldRouteSelection selection;

        EXPECT_TRUE(selection.empty());
        EXPECT_EQ(selection.size(), 0u);
    }

    TEST(AirfieldRouteSelectionTests, AddsNodesInClickOrder)
    {
        AirfieldRouteSelection selection;

        EXPECT_TRUE(selection.add("F"));
        EXPECT_TRUE(selection.add("H"));
        EXPECT_TRUE(selection.add("J"));
        EXPECT_TRUE(selection.add("18L"));

        ASSERT_EQ(selection.size(), 4u);

        EXPECT_EQ(selection.node_ids()[0], "F");
        EXPECT_EQ(selection.node_ids()[1], "H");
        EXPECT_EQ(selection.node_ids()[2], "J");
        EXPECT_EQ(selection.node_ids()[3], "18L");
    }

    TEST(AirfieldRouteSelectionTests, DuplicateNodeIsIgnored)
    {
        AirfieldRouteSelection selection;

        EXPECT_TRUE(selection.add("F"));
        EXPECT_FALSE(selection.add("F"));

        ASSERT_EQ(selection.size(), 1u);
        EXPECT_EQ(selection.node_ids()[0], "F");
    }

    TEST(AirfieldRouteSelectionTests, ClearRemovesSelectedRoute)
    {
        AirfieldRouteSelection selection;

        selection.add("F");
        selection.add("H");

        selection.clear();

        EXPECT_TRUE(selection.empty());
        EXPECT_EQ(selection.size(), 0u);
    }
}
