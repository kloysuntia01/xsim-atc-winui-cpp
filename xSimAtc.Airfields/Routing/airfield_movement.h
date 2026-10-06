#include <functional>
#pragma once
#include "airfield_waypoint.h"
#include "airfield_route.h"
#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace xsim::airfields
{
    class AirfieldMovement final
    {
    public:
        void set_completed_callback(std::function<void()> callback)
        {
            completed_callback_ = std::move(callback);
        }

        void set_route(std::vector<AirfieldWaypoint> waypoints)
        {
            completion_notified_ = false;
            waypoints_ = std::move(waypoints);
            segment_index_ = 0u;
            segment_progress_ = 0.0;
            running_ = false;
            completed_ = waypoints_.size() <= 1u;
            position_ = waypoints_.empty() ? AirfieldPosition{} : waypoints_.front().position;
        }

        [[nodiscard]] bool has_route() const noexcept { return waypoints_.size() >= 2u; }
        [[nodiscard]] bool is_running() const noexcept { return running_; }
        [[nodiscard]] bool is_completed() const noexcept { return completed_; }
        [[nodiscard]] const AirfieldPosition& position() const noexcept { return position_; }

        void start() noexcept { if (has_route() && !completed_) running_ = true; }
        void stop() noexcept { running_ = false; }

        bool advance(double segment_delta) noexcept
        {
            if (!running_ || !has_route() || completed_ || segment_delta <= 0.0) return false;
            segment_progress_ += segment_delta;
            while (segment_progress_ >= 1.0 && segment_index_ + 1u < waypoints_.size())
            {
                segment_progress_ -= 1.0;
                ++segment_index_;
                if (segment_index_ + 1u >= waypoints_.size())
                {
                    position_ = waypoints_.back().position;
                    running_ = false;
                    completed_ = true;
                notify_completed_once();
                    return true;
                }
            }
            const auto& from = waypoints_[segment_index_].position;
            const auto& to = waypoints_[segment_index_ + 1u].position;
            position_ = interpolate(from, to, std::clamp(segment_progress_, 0.0, 1.0));
            return true;
        }

    private:
        std::vector<AirfieldWaypoint> waypoints_;
        std::size_t segment_index_{0u};
        double segment_progress_{0.0};
        bool running_{false};
        bool completed_{true};
        AirfieldPosition position_{};
    
    private:
        std::function<void()> completed_callback_{};
        bool completion_notified_{ false };

        void notify_completed_once()
        {
            if (completion_notified_)
            {
                return;
            }

            completion_notified_ = true;

            if (completed_callback_)
            {
                completed_callback_();
            }
        }
};
}

