#pragma once

#include "airfield_taxiway_layout.h"

#include <array>
#include <string_view>
#include <vector>

namespace xsim::airfields::taxiway_overlay
{
    inline constexpr std::array<std::string_view, 4> ControlNodeIds{
        "F",
        "H",
        "J",
        "18L"
    };

    inline bool is_control_node(std::string_view id) noexcept
    {
        for (const auto control_id : ControlNodeIds)
        {
            if (control_id == id)
            {
                return true;
            }
        }

        return false;
    }

    inline std::vector<const taxiway_layout::TaxiwayNode*> intermediate_nodes()
    {
        std::vector<const taxiway_layout::TaxiwayNode*> result;

        for (const auto& node : taxiway_layout::Nodes)
        {
            if (!is_control_node(node.id))
            {
                result.push_back(&node);
            }
        }

        return result;
    }
}
