#include "pch.h"

#include "aircraft_mediator.h"
#include "message_queue.h"
#include "transponder_signal.h"

#include <gtest/gtest.h>
#include <objbase.h>

namespace xsim::communications::tests
{
    namespace
    {
        constexpr GUID aircraft_id{
            0x11111111,
            0x2222,
            0x3333,
            { 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb }
        };
    }

    TEST(
        TrackingStateTests,
        RegisterTrackingKeepsLatestStatePerGuid)
    {
        MessageQueue queue;

        queue.register_tracking(
            TrackingState{
                aircraft_id,
                "N123",
                1,
                100.0,
                200.0
            });

        queue.register_tracking(
            TrackingState{
                aircraft_id,
                "N123",
                2,
                125.0,
                225.0
            });

        const auto snapshot = queue.tracking_snapshot();

        ASSERT_EQ(snapshot.size(), 1u);
        EXPECT_EQ(snapshot.front().sequence, 2u);
        EXPECT_DOUBLE_EQ(snapshot.front().x, 125.0);
        EXPECT_DOUBLE_EQ(snapshot.front().y, 225.0);
    }

    TEST(
        TrackingStateTests,
        TrackingChangesPublishesRegisteredState)
    {
        MessageQueue queue;
        std::uint64_t received_sequence = 0;

        const auto subscription =
            queue
                .tracking_changes()
                .subscribe(
                    [&](const TrackingState& state)
                    {
                        received_sequence = state.sequence;
                    });

        queue.register_tracking(
            TrackingState{
                aircraft_id,
                "N123",
                7,
                100.0,
                200.0
            });

        EXPECT_EQ(received_sequence, 7u);
    }

    TEST(
        TrackingStateTests,
        AircraftMediatorRegistersTransponderWithMessageQueue)
    {
        auto queue = std::make_shared<MessageQueue>();
        AircraftMediator mediator{ queue };

        mediator.publish_transponder(
            xsim::aircrafts::TransponderSignal{
                aircraft_id,
                "FDX606",
                11,
                310.0,
                120.0,
                std::chrono::steady_clock::now()
            });

        const auto snapshot = queue->tracking_snapshot();

        ASSERT_EQ(snapshot.size(), 1u);
        EXPECT_TRUE(
            ::IsEqualGUID(
                snapshot.front().aircraft_id,
                aircraft_id));
        EXPECT_EQ(snapshot.front().call_sign, "FDX606");
        EXPECT_EQ(snapshot.front().sequence, 11u);
        EXPECT_DOUBLE_EQ(snapshot.front().x, 310.0);
        EXPECT_DOUBLE_EQ(snapshot.front().y, 120.0);
    }
}
