#pragma once

#include <string>

namespace xsim::airfields
{
    struct AirfieldEdge final
    {
        std::string from;
        std::string to;

        bool operator==(const AirfieldEdge&) const noexcept = default;
    };
}
