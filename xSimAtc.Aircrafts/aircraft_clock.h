#pragma once

#include <chrono>
#include <string>
#include <utility>

namespace xsim::aircrafts
{
    class AircraftClock final
    {
    public:
        using clock_type = std::chrono::steady_clock;
        using duration_type = std::chrono::milliseconds;

        explicit AircraftClock(std::string name)
            : name_(std::move(name))
        {
        }

        [[nodiscard]]
        const std::string& name() const noexcept
        {
            return name_;
        }

        [[nodiscard]]
        bool is_running() const noexcept
        {
            return running_;
        }

        void start() noexcept
        {
            if (running_)
            {
                return;
            }

            started_at_ = clock_type::now();
            running_ = true;
        }

        void stop() noexcept
        {
            if (!running_)
            {
                return;
            }

            accumulated_ +=
                std::chrono::duration_cast<duration_type>(
                    clock_type::now() - started_at_);

            running_ = false;
        }

        void reset() noexcept
        {
            running_ = false;
            started_at_ = {};
            accumulated_ = {};
        }

        [[nodiscard]]
        duration_type elapsed() const noexcept
        {
            if (!running_)
            {
                return accumulated_;
            }

            return accumulated_ +
                std::chrono::duration_cast<duration_type>(
                    clock_type::now() - started_at_);
        }

    private:
        std::string name_;
        bool running_{ false };
        clock_type::time_point started_at_{};
        duration_type accumulated_{};
    };
}
