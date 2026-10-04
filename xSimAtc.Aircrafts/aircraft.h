#pragma once

#include "aircraft_base.h"

#include <string>
#include <utility>

namespace xsim::aircrafts
{
    class Aircraft final : public AircraftBase
    {
    public:
        Aircraft() = default;

        explicit Aircraft(std::string callSign)
            : AircraftBase(std::move(callSign))
        {
        }
	};
    
}