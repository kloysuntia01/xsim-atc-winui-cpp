#pragma once

#include "tower_base.h"
#include "tower_signal.h"

#include <rxcpp/rx.hpp>
#include <string>

namespace xsim::towers
{
    class Tower final : public TowerBase
    {
    public:
        explicit Tower(std::string location);

        void publish_signal();

        [[nodiscard]]
        rxcpp::observable<TowerSignal> signals() const;

    private:
        rxcpp::subjects::subject<TowerSignal> subject_;
    };
}
