#pragma once

#include <guiddef.h>
#include <string>

namespace xsim::aircrafts
{
    class IAircraft
    {
    public:
        virtual ~IAircraft() = default;

        [[nodiscard]]
        virtual const GUID& id() const noexcept = 0;

        [[nodiscard]]
        virtual const std::string& call_sign() const noexcept = 0;

        virtual void tick() = 0;
    };
}
