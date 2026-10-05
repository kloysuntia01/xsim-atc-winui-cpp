#pragma once

#include "aircraft_base.h"
#include "i_aircraft_mediator.h"
#include "transponder.h"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

namespace xsim::aircrafts
{
    class Aircraft final : public AircraftBase
    {
    public:
        Aircraft() = default;

        explicit Aircraft(std::string call_sign)
            : AircraftBase(std::move(call_sign))
        {
        }

        Aircraft(
            std::string call_sign,
            std::shared_ptr<IAircraftMediator> mediator,
            double x,
            double y,
            double velocity_x,
            double velocity_y)
            : AircraftBase(call_sign, mediator),
              transponder_(
                  std::make_unique<Transponder>(
                      id(),
                      this->call_sign(),
                      std::move(mediator),
                      x,
                      y,
                      velocity_x,
                      velocity_y))
        {
        }

        ~Aircraft() override = default;

        void start_transponder(
            std::chrono::milliseconds interval =
                std::chrono::seconds{ 1 })
        {
            if (transponder_)
            {
                transponder_->start(interval);
            }
        }

        void stop_transponder()
        {
            if (transponder_)
            {
                transponder_->stop();
            }
        }

    private:
        std::unique_ptr<Transponder> transponder_;
    };
}
