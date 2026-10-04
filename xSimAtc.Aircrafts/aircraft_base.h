#pragma once

#include "i_aircraft.h"

namespace xsim::aircrafts
{
    class AircraftBase : public IAircraft
    {
    public:
        AircraftBase();

        explicit AircraftBase(
            std::string call_sign);

        [[nodiscard]]
        const GUID& id() const noexcept override;

        [[nodiscard]]
        const std::string& call_sign() const noexcept override;

    protected:
        ~AircraftBase() override = default;

    private:
        GUID id_{};
        std::string call_sign_;
    };
}
