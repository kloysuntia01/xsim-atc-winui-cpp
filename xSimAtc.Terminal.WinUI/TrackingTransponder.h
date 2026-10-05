#pragma once

#include <algorithm>
#include <string>

#include <rxcpp/rx.hpp>

namespace xsim::terminal::models
{
    struct AircraftPositionReport final
    {
        std::string fake_guid;
        std::string call_sign;
        double x{};
        double y{};
    };

    // Small first-slice transponder simulator:
    // - virtual airspace is 500 x 500 units
    // - StepAndEmit() advances one step and publishes the new position through RxCpp
    // - TrackingsViewModel calls StepAndEmit() once per second
    class TrackingTransponder final
    {
    public:
        TrackingTransponder(
            std::string fake_guid,
            std::string call_sign,
            double x,
            double y,
            double velocity_x,
            double velocity_y);

        [[nodiscard]]
        rxcpp::observable<AircraftPositionReport> Reports() const;

        [[nodiscard]]
        AircraftPositionReport CurrentReport() const;

        void StepAndEmit();

        static constexpr double Boundary = 500.0;

    private:
        void Step();

        std::string fake_guid_;
        std::string call_sign_;

        double x_{};
        double y_{};
        double velocity_x_{};
        double velocity_y_{};

        rxcpp::subjects::subject<AircraftPositionReport> reports_;
    };
}
