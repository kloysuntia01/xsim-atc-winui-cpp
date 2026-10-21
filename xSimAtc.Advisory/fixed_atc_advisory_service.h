#pragma once

#include "i_atc_advisory_service.h"

#include <string>

namespace xsim::advisory
{
    // Deterministic composition-root implementation used by the first
    // Tower advisory UI boundary slice. It proves the UI/ViewModel/service
    // seam without introducing model timing into the WinUI smoke.
    class FixedAtcAdvisoryService final
        : public IAtcAdvisoryService
    {
    public:
        [[nodiscard]]
        AtcAdvisoryResult advise(
            const AtcAdvisoryRequest& request) const override
        {
            AtcAdvisoryResult result;

            if (request.runway == "18R" &&
                request.runway_status == "Occupied" &&
                request.call_sign == "FDX606" &&
                request.aircraft_status == "Holding Short")
            {
                result.text =
                    "Hold FDX606 short. Runway 18R is occupied.";
            }
            else
            {
                result.text =
                    "Maintain current state and request controller review.";
            }

            result.world_state_changed = false;
            return result;
        }
    };
}
