#pragma once

#include "i_aircraft_mediator.h"
#include "transponder_signal.h"

#include <chrono>
#include <cstdint>
#include <guiddef.h>
#include <memory>
#include <rxcpp/rx.hpp>
#include <string>
#include <thread>

namespace xsim::aircrafts
{
    class Transponder final
    {
    public:
        Transponder(
            const GUID& aircraft_id,
            std::string call_sign);

        Transponder(
            const GUID& aircraft_id,
            std::string call_sign,
            std::shared_ptr<IAircraftMediator> mediator,
            double x,
            double y,
            double velocity_x,
            double velocity_y);

        ~Transponder();

        Transponder(const Transponder&) = delete;
        Transponder& operator=(const Transponder&) = delete;

        void start(
            std::chrono::milliseconds interval =
                std::chrono::seconds{ 1 });

        void stop();

        [[nodiscard]]
        rxcpp::observable<TransponderSignal> signals() const;

    private:
        void run(
            std::stop_token stop_token,
            std::chrono::milliseconds interval);

        void step_position();

        static constexpr double boundary_ = 500.0;

        GUID aircraft_id_{};
        std::string call_sign_;
        std::shared_ptr<IAircraftMediator> mediator_;

        std::uint64_t sequence_{};
        double x_{};
        double y_{};
        double velocity_x_{};
        double velocity_y_{};

        rxcpp::subjects::subject<TransponderSignal> subject_;
        std::jthread worker_;
    };
}
