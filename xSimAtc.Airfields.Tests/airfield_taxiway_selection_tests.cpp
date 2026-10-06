#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxiway_selection.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiwaySelectionTests, EveryKnownTaxiwayNodeIsSelectable)
    {
        for (const auto& node : taxiway_layout::Nodes)
        {
            EXPECT_TRUE(
                taxiway_selection::is_selectable(node.id))
                << "Expected selectable node " << node.id;
        }

        EXPECT_FALSE(
            taxiway_selection::is_selectable("NOT-A-NODE"));
    }

    TEST(AirfieldTaxiwaySelectionTests, SelectableNodesExposeEntireOptionBNetwork)
    {
        const auto nodes =
            taxiway_selection::selectable_nodes();

        ASSERT_EQ(
            nodes.size(),
            taxiway_layout::Nodes.size());

        EXPECT_EQ(nodes.front()->id, "F");
        EXPECT_EQ(nodes.back()->id, "S");
    }
}
