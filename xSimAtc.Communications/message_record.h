#pragma once

#include "pch.h"
//#include <guiddef.h>
//#include <string>

namespace xsim::communications
{
    struct MessageRecord final
    {
        GUID sender_id{};
        GUID receiver_id{};
        std::string message{};
    };
}
