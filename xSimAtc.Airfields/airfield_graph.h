#pragma once

#include "airfield_edge.h"

#include <algorithm>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace xsim::airfields
{
    class AirfieldGraph final
    {
    public:
        bool add_node(std::string node_id)
        {
            if (node_id.empty())
            {
                return false;
            }

            return adjacency_.try_emplace(std::move(node_id)).second;
        }

        bool contains_node(std::string_view node_id) const
        {
            return adjacency_.contains(std::string{ node_id });
        }

        bool add_edge(std::string from, std::string to)
        {
            if (from.empty() || to.empty() || from == to)
            {
                return false;
            }

            add_node(from);
            add_node(to);

            auto& from_neighbors = adjacency_.at(from);
            auto& to_neighbors = adjacency_.at(to);

            if (std::find(from_neighbors.begin(), from_neighbors.end(), to) != from_neighbors.end())
            {
                return false;
            }

            from_neighbors.push_back(to);
            to_neighbors.push_back(from);

            edges_.push_back(AirfieldEdge{
                std::move(from),
                std::move(to)
            });

            return true;
        }

        bool connected(std::string_view from, std::string_view to) const
        {
            const auto found = adjacency_.find(std::string{ from });
            if (found == adjacency_.end())
            {
                return false;
            }

            return std::find(
                found->second.begin(),
                found->second.end(),
                std::string{ to }) != found->second.end();
        }

        [[nodiscard]]
        const std::vector<AirfieldEdge>& edges() const noexcept
        {
            return edges_;
        }

        [[nodiscard]]
        std::vector<std::string> route(
            std::string_view from,
            std::string_view to) const
        {
            const std::string start{ from };
            const std::string finish{ to };

            if (!adjacency_.contains(start) || !adjacency_.contains(finish))
            {
                return {};
            }

            if (start == finish)
            {
                return { start };
            }

            std::queue<std::string> pending;
            std::unordered_set<std::string> visited;
            std::unordered_map<std::string, std::string> previous;

            pending.push(start);
            visited.insert(start);

            while (!pending.empty())
            {
                auto current = std::move(pending.front());
                pending.pop();

                for (const auto& neighbor : adjacency_.at(current))
                {
                    if (!visited.insert(neighbor).second)
                    {
                        continue;
                    }

                    previous.emplace(neighbor, current);

                    if (neighbor == finish)
                    {
                        return rebuild_route(start, finish, previous);
                    }

                    pending.push(neighbor);
                }
            }

            return {};
        }

    private:
        static std::vector<std::string> rebuild_route(
            const std::string& start,
            const std::string& finish,
            const std::unordered_map<std::string, std::string>& previous)
        {
            std::vector<std::string> result;
            auto current = finish;

            result.push_back(current);

            while (current != start)
            {
                const auto found = previous.find(current);
                if (found == previous.end())
                {
                    return {};
                }

                current = found->second;
                result.push_back(current);
            }

            std::reverse(result.begin(), result.end());
            return result;
        }

        std::unordered_map<std::string, std::vector<std::string>> adjacency_;
        std::vector<AirfieldEdge> edges_;
    };
}
