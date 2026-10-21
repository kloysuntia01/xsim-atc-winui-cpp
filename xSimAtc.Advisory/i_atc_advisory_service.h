#pragma once

#include "atc_advisory_request.h"
#include "atc_advisory_result.h"

namespace xsim::advisory
{
    class IAtcAdvisoryService
    {
    public:
        virtual ~IAtcAdvisoryService() = default;

        [[nodiscard]]
        virtual AtcAdvisoryResult advise(
            const AtcAdvisoryRequest& request) const = 0;
    };
}
