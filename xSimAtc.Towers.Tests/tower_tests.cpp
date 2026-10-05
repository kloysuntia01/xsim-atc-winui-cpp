#include "pch.h"

#include "tower.h"

#include <cstring>
#include <string>

namespace xsim::towers::tests
{
    namespace
    {
        bool same_guid(
            const GUID& left,
            const GUID& right) noexcept
        {
            return std::memcmp(
                &left,
                &right,
                sizeof(GUID)) == 0;
        }
    }

    TEST(TowerTests, ConstructorPreservesLocation)
    {
        Tower tower{ "north" };

        EXPECT_EQ(tower.location(), "north");
    }

    TEST(TowerTests, ConstructorGeneratesNonEmptyGuid)
    {
        Tower tower{ "north" };
        const GUID empty_guid{};

        EXPECT_FALSE(same_guid(tower.id(), empty_guid));
    }

    TEST(TowerTests, TwoTowersGenerateDifferentGuids)
    {
        Tower north{ "north" };
        Tower south{ "south" };

        EXPECT_FALSE(same_guid(north.id(), south.id()));
    }

    TEST(TowerTests, PublishedSignalContainsLocation)
    {
        Tower tower{ "east" };
        std::string received_location;

        const auto subscription =
            tower.signals().subscribe(
                [&](const TowerSignal& signal)
                {
                    received_location = signal.location;
                });

        tower.publish_signal();

        EXPECT_EQ(received_location, "east");
    }

    TEST(TowerTests, PublishedSignalContainsTowerGuid)
    {
        Tower tower{ "west" };
        GUID received_id{};

        const auto subscription =
            tower.signals().subscribe(
                [&](const TowerSignal& signal)
                {
                    received_id = signal.tower_id;
                });

        tower.publish_signal();

        EXPECT_TRUE(same_guid(received_id, tower.id()));
    }

    TEST(TowerTests, TwoTowersHaveIndependentStreams)
    {
        Tower north{ "north" };
        Tower south{ "south" };
        int north_signal_count = 0;
        int south_signal_count = 0;

        const auto north_subscription =
            north.signals().subscribe(
                [&](const TowerSignal&)
                {
                    ++north_signal_count;
                });

        const auto south_subscription =
            south.signals().subscribe(
                [&](const TowerSignal&)
                {
                    ++south_signal_count;
                });

        north.publish_signal();

        EXPECT_EQ(north_signal_count, 1);
        EXPECT_EQ(south_signal_count, 0);

        south.publish_signal();

        EXPECT_EQ(north_signal_count, 1);
        EXPECT_EQ(south_signal_count, 1);
    }
}
