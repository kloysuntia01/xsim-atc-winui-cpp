#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_overlay.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiwayOverlayTests, ClassifiesControllerNodes)
    {
        EXPECT_TRUE(taxiway_overlay::is_control_node("F"));
        EXPECT_TRUE(taxiway_overlay::is_control_node("H"));
        EXPECT_TRUE(taxiway_overlay::is_control_node("J"));
        EXPECT_TRUE(taxiway_overlay::is_control_node("18L"));

        EXPECT_FALSE(taxiway_overlay::is_control_node("F1"));
        EXPECT_FALSE(taxiway_overlay::is_control_node("H1"));
        EXPECT_FALSE(taxiway_overlay::is_control_node("J1"));
    }

    TEST(AirfieldTaxiwayOverlayTests, ReturnsOnlyIntermediateTaxiwayNodes)
    {
        const auto nodes = taxiway_overlay::intermediate_nodes();

        ASSERT_EQ(nodes.size(), 16u);

        const auto contains =
            [&nodes](std::string_view id)
            {
                for (const auto* node : nodes)
                {
                    if (node->id == id)
                    {
                        return true;
                    }
                }

                return false;
            };

        for (const auto* id : {
            "F1", "H1", "J1",
            "B", "C", "D", "E", "G", "K",
            "L", "M", "N", "P", "Q", "R", "S" })
        {
            EXPECT_TRUE(contains(id))
                << "Missing intermediate overlay node " << id;
        }
    }
}

