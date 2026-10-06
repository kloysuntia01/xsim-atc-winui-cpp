#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_layout.h"

#include <string>
#include <vector>

namespace xsim::airfields::tests
{
    TEST(AirfieldFullTaxiwayNetworkTests, ContainsExtendedOptionBIntersections)
    {
        for (const auto* id : {
            "B", "C", "D", "E", "G", "K",
            "L", "M", "N", "P", "Q", "R", "S" })
        {
            EXPECT_NE(
                taxiway_layout::find_node(id),
                nullptr)
                << "Missing taxiway node " << id;
        }
    }

    TEST(AirfieldFullTaxiwayNetworkTests, ExtendedNetworkIsConnectedToPrimaryRoute)
    {
        const auto graph = taxiway_layout::build_graph();

        EXPECT_FALSE(graph.route("B", "18L").empty());
        EXPECT_FALSE(graph.route("D", "18L").empty());
        EXPECT_FALSE(graph.route("P", "F").empty());
        EXPECT_FALSE(graph.route("S", "J").empty());
    }

    TEST(AirfieldFullTaxiwayNetworkTests, PrimaryControllerRouteStillUsesPhysicalIntersections)
    {
        const auto graph = taxiway_layout::build_graph();

        const std::vector<std::string> selected{
            "F",
            "H",
            "J",
            "18L"
        };

        const auto expanded =
            taxiway_layout::expand_control_route(
                graph,
                selected);

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

    TEST(AirfieldFullTaxiwayNetworkTests, ExtendedNetworkRejectsUnknownDestination)
    {
        const auto graph = taxiway_layout::build_graph();

        EXPECT_TRUE(
            graph.route(
                "B",
                "NOT-A-NODE").empty());
    }
}
