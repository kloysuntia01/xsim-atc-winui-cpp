#pragma once

#include "../Collections/collection.h"
#include "../Positions/airfield_position.h"

#include <string>
#include <utility>

namespace xsim::airfields
{
    enum class AirfieldNodeKind
    {
        TaxiPoint,
        RunwayEnd
    };

    struct AirfieldNode final
    {
        std::string id;
        AirfieldNodeKind kind{ AirfieldNodeKind::TaxiPoint };
        AirfieldPosition position{};

        AirfieldNode() = default;

        AirfieldNode(
            std::string node_id,
            AirfieldNodeKind node_kind,
            AirfieldPosition node_position)
            : id(std::move(node_id)),
              kind(node_kind),
              position(node_position)
        {
        }
    };

    using AirfieldNodes =
        Collection<AirfieldNode>;
}
