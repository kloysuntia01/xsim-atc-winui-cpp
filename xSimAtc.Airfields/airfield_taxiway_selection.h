#pragma once

#include "airfield_taxiway_layout.h"

#include <string_view>
#include <vector>

namespace xsim::airfields::taxiway_selection
{
    inline bool is_selectable(std::string_view id) noexcept
    {
        return taxiway_layout::find_node(id) != nullptr;
    }

    inline std::vector<const taxiway_layout::TaxiwayNode*> selectable_nodes()
    {
        std::vector<const taxiway_layout::TaxiwayNode*> result;
        result.reserve(taxiway_layout::Nodes.size());

        for (const auto& node : taxiway_layout::Nodes)
        {
            result.push_back(&node);
        }

        return result;
    }
}
