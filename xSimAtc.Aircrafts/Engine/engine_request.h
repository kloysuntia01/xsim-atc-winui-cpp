#pragma once

namespace xsim::aircrafts
{
    struct EngineRequest final
    {
        double throttle{ 0.65 };
        double fuel{ 100.0 };
        bool engine_running{ true };
        double speed{};
        double x{ 250.0 };
        double y{ 250.0 };
        double velocity_x{ 1.0 };
        double velocity_y{ 1.0 };
    };
}
