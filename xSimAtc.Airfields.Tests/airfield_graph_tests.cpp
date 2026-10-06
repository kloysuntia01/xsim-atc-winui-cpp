#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_graph.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldGraphTests, AddsUndirectedTaxiwayEdge)
    {
        AirfieldGraph graph;

        EXPECT_TRUE(graph.add_edge("F", "F1"));
        EXPECT_TRUE(graph.connected("F", "F1"));
        EXPECT_TRUE(graph.connected("F1", "F"));
    }

    TEST(AirfieldGraphTests, DuplicateEdgeIsIgnored)
    {
        AirfieldGraph graph;

        EXPECT_TRUE(graph.add_edge("F", "F1"));
        EXPECT_FALSE(graph.add_edge("F", "F1"));
        EXPECT_FALSE(graph.add_edge("F1", "F"));

        EXPECT_EQ(graph.edges().size(), 1u);
    }

    TEST(AirfieldGraphTests, FindsShortestTaxiwayRoute)
    {
        AirfieldGraph graph;

        graph.add_edge("F", "F1");
        graph.add_edge("F1", "H");
        graph.add_edge("H", "H1");
        graph.add_edge("H1", "J");
        graph.add_edge("J", "J1");
        graph.add_edge("J1", "18L");

        const auto route = graph.route("F", "18L");

        const std::vector<std::string> expected{
            "F",
            "F1",
            "H",
            "H1",
            "J",
            "J1",
            "18L"
        };

        EXPECT_EQ(route, expected);
    }

    TEST(AirfieldGraphTests, ReturnsEmptyRouteWhenDisconnected)
    {
        AirfieldGraph graph;

        graph.add_edge("F", "H");
        graph.add_node("18L");

        EXPECT_TRUE(graph.route("F", "18L").empty());
    }
}
