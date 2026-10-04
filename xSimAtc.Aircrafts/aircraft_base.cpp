#include "pch.h"
#include "aircraft_base.h"

namespace xsim::aircrafts
{
    AircraftBase::AircraftBase()
        : AircraftBase("")
    {
    }

    AircraftBase::AircraftBase(
        std::string call_sign)
        : call_sign_(std::move(call_sign))
    {
        const auto result =
            ::CoCreateGuid(&id_);

        if (FAILED(result))
        {
            throw std::runtime_error(
                "Unable to generate aircraft GUID.");
        }
    }

    const GUID&
        AircraftBase::id() const noexcept
    {
        return id_;
    }

    const std::string&
        AircraftBase::call_sign() const noexcept
    {
        return call_sign_;
    }
}