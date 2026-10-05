#include "pch.h"

#include "communication_checkpoint.h"



namespace xsim::communications::tests
{
    namespace
    {
        constexpr GUID sender_id{
            0x10000001,
            0x1001,
            0x1001,
            { 0x10, 0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x01 }
        };

        constexpr GUID receiver_id{
            0x20000002,
            0x2002,
            0x2002,
            { 0x20, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x02 }
        };

        constexpr GUID other_id{
            0x30000003,
            0x3003,
            0x3003,
            { 0x30, 0x03, 0x30, 0x03, 0x30, 0x03, 0x30, 0x03 }
        };
    }

    TEST(
        CommunicationCheckpointTests,
        SendPreservesSenderGuid)
    {
        CommunicationCheckpoint checkpoint;
        GUID received_sender{};

        const auto subscription =
            checkpoint.records().subscribe(
                [&](const CommunicationRecord& record)
                {
                    received_sender = record.sender_id;
                });

        checkpoint.send(sender_id, receiver_id);

        EXPECT_TRUE(
            ::IsEqualGUID(received_sender, sender_id));
    }

    TEST(
        CommunicationCheckpointTests,
        SendPreservesReceiverGuid)
    {
        CommunicationCheckpoint checkpoint;
        GUID received_receiver{};

        const auto subscription =
            checkpoint.records().subscribe(
                [&](const CommunicationRecord& record)
                {
                    received_receiver = record.receiver_id;
                });

        checkpoint.send(sender_id, receiver_id);

        EXPECT_TRUE(
            ::IsEqualGUID(received_receiver, receiver_id));
    }

    TEST(
        CommunicationCheckpointTests,
        SubscriberReceivesRecord)
    {
        CommunicationCheckpoint checkpoint;
        int received_count = 0;

        const auto subscription =
            checkpoint.records().subscribe(
                [&](const CommunicationRecord&)
                {
                    ++received_count;
                });

        checkpoint.send(sender_id, receiver_id);

        EXPECT_EQ(received_count, 1);
    }

    TEST(
        CommunicationCheckpointTests,
        SubscriberCanFilterByOwnGuid)
    {
        CommunicationCheckpoint checkpoint;
        int received_count = 0;

        const auto subscription =
            checkpoint
                .records()
                .filter(
                    [](const CommunicationRecord& record)
                    {
                        return ::IsEqualGUID(
                            record.receiver_id,
                            receiver_id);
                    })
                .subscribe(
                    [&](const CommunicationRecord&)
                    {
                        ++received_count;
                    });

        checkpoint.send(sender_id, receiver_id);

        EXPECT_EQ(received_count, 1);
    }

    TEST(
        CommunicationCheckpointTests,
        WrongGuidDoesNotReceiveFilteredRecord)
    {
        CommunicationCheckpoint checkpoint;
        int received_count = 0;

        const auto subscription =
            checkpoint
                .records()
                .filter(
                    [](const CommunicationRecord& record)
                    {
                        return ::IsEqualGUID(
                            record.receiver_id,
                            other_id);
                    })
                .subscribe(
                    [&](const CommunicationRecord&)
                    {
                        ++received_count;
                    });

        checkpoint.send(sender_id, receiver_id);

        EXPECT_EQ(received_count, 0);
    }

    TEST(
        CommunicationCheckpointTests,
        SeparateCheckpointsRemainIndependent)
    {
        CommunicationCheckpoint first;
        CommunicationCheckpoint second;
        int first_count = 0;
        int second_count = 0;

        const auto first_subscription =
            first.records().subscribe(
                [&](const CommunicationRecord&)
                {
                    ++first_count;
                });

        const auto second_subscription =
            second.records().subscribe(
                [&](const CommunicationRecord&)
                {
                    ++second_count;
                });

        first.send(sender_id, receiver_id);

        EXPECT_EQ(first_count, 1);
        EXPECT_EQ(second_count, 0);

        second.send(receiver_id, sender_id);

        EXPECT_EQ(first_count, 1);
        EXPECT_EQ(second_count, 1);
    }
}
