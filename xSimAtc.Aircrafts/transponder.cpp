#include "pch.h"
#include "transponder.h"

#include <algorithm>

namespace xsim::aircrafts
{
    using std::chrono::milliseconds;
    using std::move;
    using std::stop_token;

    Transponder::Transponder(
        const GUID& aircraft_id,
        std::string call_sign)
        : Transponder(
            aircraft_id,
            move(call_sign),
            nullptr,
            250.0,
            250.0,
            0.0,
            0.0)
    {
    }

    Transponder::Transponder(
        const GUID& aircraft_id,
        std::string call_sign,
        std::shared_ptr<IAircraftMediator> mediator,
        double x,
        double y,
        double velocity_x,
        double velocity_y)
        : aircraft_id_(aircraft_id),
          call_sign_(move(call_sign)),
          mediator_(move(mediator)),
          x_(x),
          y_(y),
          velocity_x_(velocity_x),
          velocity_y_(velocity_y)
    {
    }

    Transponder::~Transponder()
    {
        stop();
    }

    void Transponder::start(milliseconds interval)
    {
        stop();

        worker_ =
            std::jthread(
                [this, interval](stop_token stop_token)
                {
                    run(stop_token, interval);
                });
    }

    void Transponder::stop()
    {
        if (worker_.joinable())
        {
            worker_.request_stop();
            worker_.join();
        }
    }

    rxcpp::observable<TransponderSignal>
        Transponder::signals() const
    {
        return subject_
            .get_observable()
            .as_dynamic();
    }

    void Transponder::run(
        stop_token stop_token,
        milliseconds interval)
    {
        while (!stop_token.stop_requested())
        {
            step_position();

            TransponderSignal signal{
                aircraft_id_,
                call_sign_,
                ++sequence_,
                x_,
                y_,
                std::chrono::steady_clock::now()
            };

            if (mediator_)
            {
                mediator_->publish_transponder(signal);
            }

            subject_
                .get_subscriber()
                .on_next(signal);

            std::this_thread::sleep_for(interval);
        }
    }

    void Transponder::step_position()
    {
        auto next_x = x_ + velocity_x_;
        auto next_y = y_ + velocity_y_;

        if (next_x < 0.0 || next_x > boundary_)
        {
            velocity_x_ = -velocity_x_;
            next_x = x_ + velocity_x_;
        }

        if (next_y < 0.0 || next_y > boundary_)
        {
            velocity_y_ = -velocity_y_;
            next_y = y_ + velocity_y_;
        }

        x_ = std::clamp(next_x, 0.0, boundary_);
        y_ = std::clamp(next_y, 0.0, boundary_);
    }
}
