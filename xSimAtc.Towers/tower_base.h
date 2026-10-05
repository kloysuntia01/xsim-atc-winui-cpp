#pragma once

#include "i_tower.h"

namespace xsim::towers
{
    class TowerBase : public ITower
    {
    public:
        explicit TowerBase(std::string location);

        [[nodiscard]]
        const GUID& id() const noexcept override;

        [[nodiscard]]
        const std::string& location() const noexcept override;

    protected:
        ~TowerBase() override = default;

    private:
        GUID id_{};
        std::string location_;
    };
}
