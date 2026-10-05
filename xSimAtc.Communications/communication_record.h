#pragma once
#include "pch.h"
//#include <guiddef.h>

namespace xsim::communications
{
    struct CommunicationRecord final
    {
        GUID sender_id{};
        GUID receiver_id{};
    };
}
