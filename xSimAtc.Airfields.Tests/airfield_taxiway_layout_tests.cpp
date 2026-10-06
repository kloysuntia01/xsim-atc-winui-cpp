#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_layout.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiwayLayoutTests, ContainsLogicalAndPhysicalTaxiwayNodes)
    {
        EXPECT_NE(taxiway_layout::find_node("F"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("F1"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("H"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("H1"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("J"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("J1"), nullptr);
        EXPECT_NE(taxiway_layout::find_node("18L"), nullptr);
    }

    TEST(AirfieldTaxiwayLayoutTests, BuildsConnectedOptionBTaxiwayGraph)
    {
        const auto graph = taxiway_layout::build_graph();

        EXPECT_TRUE(graph.connected("F", "F1"));
        EXPECT_TRUE(graph.connected("F1", "H"));
        EXPECT_TRUE(graph.connected("H", "H1"));
        EXPECT_TRUE(graph.connected("H1", "J"));
        EXPECT_TRUE(graph.connected("J", "J1"));
        EXPECT_TRUE(graph.connected("J1", "18L"));
    }

    TEST(AirfieldTaxiwayLayoutTests, ExpandsControllerRouteThroughTaxiwayIntersections)
    {
        const auto graph = taxiway_layout::build_graph();

        const std::vector<std::string> selected{
            "F",
            "H",
            "J",
            "18L"
        };

        const auto expanded =
            taxiway_layout::expand_control_route(graph, selected);

        const std::vector<std::string> expected{
            "F",
            "F1",
            "H",
            "H1",
            "J",
            "J1",
            "18L"
        };

        EXPECT_EQ(expanded, expected);
    }

    TEST(AirfieldTaxiwayLayoutTests, ExpandedRouteDoesNotDuplicateSegmentJoinNodes)
    {
        const auto graph = taxiway_layout::build_graph();

        const std::vector<std::string> selected{
            "F",
            "H",
            "J"
        };

        const auto expanded =
            taxiway_layout::expand_control_route(graph, selected);

        const std::vector<std::string> expected{
            "F",
            "F1",
            "H",
            "H1",
            "J"
        };

        EXPECT_EQ(expanded, expected);
    }
}
