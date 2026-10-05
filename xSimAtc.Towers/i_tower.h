#pragma once

#include <guiddef.h>
#include <string>

namespace xsim::towers
{
    class ITower
    {
    public:
        virtual ~ITower() = default;

        [[nodiscard]]
        virtual const GUID& id() const noexcept = 0;

        [[nodiscard]]
        virtual const std::string& location() const noexcept = 0;
    };
}
