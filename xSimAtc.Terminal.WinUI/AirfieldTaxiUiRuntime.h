#pragma once

#include "../xSimAtc.Airfields/airfield_taxi_runtime.h"

namespace xSimAtc_Terminal_WinUI
{
    class AirfieldTaxiUiRuntime final
    {
    public:
        [[nodiscard]]
        xsim::airfields::AirfieldTaxiRuntime& taxi_runtime() noexcept
        {
            return taxi_runtime_;
        }

        [[nodiscard]]
        const xsim::airfields::AirfieldTaxiRuntime& taxi_runtime() const noexcept
        {
            return taxi_runtime_;
        }

    private:
        xsim::airfields::AirfieldTaxiRuntime taxi_runtime_;
    };
}
