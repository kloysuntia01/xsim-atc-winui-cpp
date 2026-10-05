#pragma once

#include <cstdint>
#include <guiddef.h>
#include <string>

namespace xsim::aircrafts
{
    enum class CommunicationState
    {
        Hailing,
        Active,
        HandingOff,
        Completed
    };

    [[nodiscard]]
    inline bool guid_is_empty(const GUID& value) noexcept
    {
        return value.Data1 == 0 &&
            value.Data2 == 0 &&
            value.Data3 == 0 &&
            value.Data4[0] == 0 &&
            value.Data4[1] == 0 &&
            value.Data4[2] == 0 &&
            value.Data4[3] == 0 &&
            value.Data4[4] == 0 &&
            value.Data4[5] == 0 &&
            value.Data4[6] == 0 &&
            value.Data4[7] == 0;
    }

    struct CommunicationRequest final
    {
        GUID conversation_id{};
        GUID originator_guid{};
        GUID sender_guid{};
        GUID receiver_guid{};
        GUID current_peer_guid{};
        GUID handoff_guid{};

        std::uint64_t sequence{};
        CommunicationState state{ CommunicationState::Hailing };
        std::string message;
    };
}
