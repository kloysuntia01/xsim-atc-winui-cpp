#include "pch.h"
#include "tower_base.h"

namespace xsim::towers
{
    TowerBase::TowerBase(std::string location)
        : location_(std::move(location))
    {
        const auto result = ::CoCreateGuid(&id_);

        if (FAILED(result))
        {
            throw std::runtime_error(
                "Unable to generate tower GUID.");
        }
    }

    const GUID& TowerBase::id() const noexcept
    {
        return id_;
    }

    const std::string& TowerBase::location() const noexcept
    {
        return location_;
    }
}
