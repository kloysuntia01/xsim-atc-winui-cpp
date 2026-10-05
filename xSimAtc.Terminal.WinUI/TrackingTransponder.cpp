#include "pch.h"
#include "TrackingTransponder.h"

namespace xsim::terminal::models
{
    TrackingTransponder::TrackingTransponder(
        std::string fake_guid,
        std::string call_sign,
        double x,
        double y,
        double velocity_x,
        double velocity_y)
        : fake_guid_(std::move(fake_guid)),
          call_sign_(std::move(call_sign)),
          x_(std::clamp(x, 0.0, Boundary)),
          y_(std::clamp(y, 0.0, Boundary)),
          velocity_x_(velocity_x),
          velocity_y_(velocity_y)
    {
    }

    rxcpp::observable<AircraftPositionReport>
    TrackingTransponder::Reports() const
    {
        return reports_
            .get_observable()
            .as_dynamic();
    }

    AircraftPositionReport
    TrackingTransponder::CurrentReport() const
    {
        return AircraftPositionReport{
            fake_guid_,
            call_sign_,
            x_,
            y_
        };
    }

    void TrackingTransponder::StepAndEmit()
    {
        Step();

        reports_
            .get_subscriber()
            .on_next(CurrentReport());
    }

    void TrackingTransponder::Step()
    {
        x_ += velocity_x_;
        y_ += velocity_y_;

        if (x_ < 0.0)
        {
            x_ = -x_;
            velocity_x_ = -velocity_x_;
        }
        else if (x_ > Boundary)
        {
            x_ = Boundary - (x_ - Boundary);
            velocity_x_ = -velocity_x_;
        }

        if (y_ < 0.0)
        {
            y_ = -y_;
            velocity_y_ = -velocity_y_;
        }
        else if (y_ > Boundary)
        {
            y_ = Boundary - (y_ - Boundary);
            velocity_y_ = -velocity_y_;
        }

        x_ = std::clamp(x_, 0.0, Boundary);
        y_ = std::clamp(y_, 0.0, Boundary);
    }
}
