#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace xsim::aircrafts
{
    template<typename T>
    class Collection
    {
    public:
        using value_type = T;
        using storage_type = std::vector<T>;
        using iterator = typename storage_type::iterator;
        using const_iterator = typename storage_type::const_iterator;

        [[nodiscard]]
        bool empty() const noexcept
        {
            return items_.empty();
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return items_.size();
        }

        void add(T item)
        {
            items_.push_back(std::move(item));
        }

        iterator begin() noexcept
        {
            return items_.begin();
        }

        iterator end() noexcept
        {
            return items_.end();
        }

        const_iterator begin() const noexcept
        {
            return items_.begin();
        }

        const_iterator end() const noexcept
        {
            return items_.end();
        }

        const_iterator cbegin() const noexcept
        {
            return items_.cbegin();
        }

        const_iterator cend() const noexcept
        {
            return items_.cend();
        }

    private:
        storage_type items_;
    };
}
