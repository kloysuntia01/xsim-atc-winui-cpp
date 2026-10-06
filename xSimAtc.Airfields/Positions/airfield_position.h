#pragma once

#include "../Collections/collection.h"

namespace xsim::airfields
{
    struct AirfieldPosition final
    {
        double x{};
        double y{};

        [[nodiscard]]
        constexpr bool operator==(
            const AirfieldPosition&) const noexcept = default;
    };

    using AirfieldPositions =
        Collection<AirfieldPosition>;
}
