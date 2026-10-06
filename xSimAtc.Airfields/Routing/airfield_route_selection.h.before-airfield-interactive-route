#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace xsim::airfields
{
    class AirfieldRouteSelection final
    {
    public:
        [[nodiscard]]
        bool empty() const noexcept
        {
            return node_ids_.empty();
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return node_ids_.size();
        }

        [[nodiscard]]
        const std::vector<std::string>& node_ids() const noexcept
        {
            return node_ids_;
        }

        [[nodiscard]]
        bool contains(std::string_view node_id) const
        {
            return std::ranges::any_of(
                node_ids_,
                [node_id](const std::string& existing)
                {
                    return existing == node_id;
                });
        }

        bool add(std::string node_id)
        {
            if (node_id.empty() || contains(node_id))
            {
                return false;
            }

            node_ids_.push_back(std::move(node_id));
            return true;
        }

        void clear() noexcept
        {
            node_ids_.clear();
        }

    private:
        std::vector<std::string> node_ids_;
    };
}
