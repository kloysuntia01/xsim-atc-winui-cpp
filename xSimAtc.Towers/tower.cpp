#include "pch.h"
#include "tower.h"

namespace xsim::towers
{
    Tower::Tower(std::string location)
        : TowerBase(std::move(location))
    {
    }

    void Tower::publish_signal()
    {
        subject_
            .get_subscriber()
            .on_next(
                TowerSignal{
                    id(),
                    location(),
                    std::chrono::steady_clock::now()
                });
    }

    rxcpp::observable<TowerSignal> Tower::signals() const
    {
        return subject_
            .get_observable()
            .as_dynamic();
    }
}
