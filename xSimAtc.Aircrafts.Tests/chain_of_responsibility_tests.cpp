#include "pch.h"

#include "Communication/hailing_handler.h"
#include "Communication/handoff_handler.h"
#include "Communication/pairing_handler.h"
#include "aircraft.h"

#include <gtest/gtest.h>
#include <objbase.h>

namespace
{
    GUID make_guid()
    {
        GUID value{};
        EXPECT_TRUE(SUCCEEDED(::CoCreateGuid(&value)));
        return value;
    }
}

TEST(AircraftEngineChainTests, TickProcessesEngineChain)
{
    xsim::aircrafts::Aircraft aircraft{ "N123" };

    const auto before = aircraft.engine_state();
    aircraft.tick();
    const auto after = aircraft.engine_state();

    EXPECT_LT(after.fuel, before.fuel);
    EXPECT_GT(after.speed, 0.0);
    EXPECT_GT(after.x, before.x);
    EXPECT_GT(after.y, before.y);
}

TEST(CommunicationChainTests, HailingSetsOriginatorFromSender)
{
    const auto aircraft_id = make_guid();

    xsim::aircrafts::CommunicationRequest request{};
    request.sender_guid = aircraft_id;
    request.state = xsim::aircrafts::CommunicationState::Hailing;

    xsim::aircrafts::HailingHandler handler;
    handler.handle(request);

    EXPECT_TRUE(::IsEqualGUID(request.originator_guid, aircraft_id));
}

TEST(CommunicationChainTests, PairingActivatesReceiverAsCurrentPeer)
{
    const auto tower_id = make_guid();

    xsim::aircrafts::CommunicationRequest request{};
    request.receiver_guid = tower_id;
    request.state = xsim::aircrafts::CommunicationState::Hailing;

    xsim::aircrafts::PairingHandler handler;
    handler.handle(request);

    EXPECT_EQ(request.state, xsim::aircrafts::CommunicationState::Active);
    EXPECT_TRUE(::IsEqualGUID(request.current_peer_guid, tower_id));
}

TEST(CommunicationChainTests, HandoffChangesPeerWithoutChangingConversation)
{
    const auto conversation_id = make_guid();
    const auto old_tower_id = make_guid();
    const auto new_tower_id = make_guid();

    xsim::aircrafts::CommunicationRequest request{};
    request.conversation_id = conversation_id;
    request.current_peer_guid = old_tower_id;
    request.handoff_guid = new_tower_id;
    request.state = xsim::aircrafts::CommunicationState::HandingOff;

    xsim::aircrafts::HandoffHandler handler;
    handler.handle(request);

    EXPECT_TRUE(::IsEqualGUID(request.conversation_id, conversation_id));
    EXPECT_TRUE(::IsEqualGUID(request.current_peer_guid, new_tower_id));
    EXPECT_TRUE(::IsEqualGUID(request.receiver_guid, new_tower_id));
    EXPECT_EQ(request.state, xsim::aircrafts::CommunicationState::Active);
}
