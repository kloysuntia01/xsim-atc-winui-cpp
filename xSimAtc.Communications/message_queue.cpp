#include "pch.h"
#include "message_queue.h"

#include <algorithm>
#include <objbase.h>

namespace xsim::communications
{
    void MessageQueue::send(const MessageRecord& record)
    {
        subject_
            .get_subscriber()
            .on_next(record);
    }

    rxcpp::observable<MessageRecord> MessageQueue::records() const
    {
        return subject_
            .get_observable()
            .as_dynamic();
    }

    void MessageQueue::register_tracking(
        const TrackingState& state)
    {
        {
            std::scoped_lock lock{ tracking_mutex_ };

            const auto existing =
                std::find_if(
                    tracking_states_.begin(),
                    tracking_states_.end(),
                    [&](const TrackingState& current)
                    {
                        return ::IsEqualGUID(
                            current.aircraft_id,
                            state.aircraft_id);
                    });

            if (existing == tracking_states_.end())
            {
                tracking_states_.push_back(state);
            }
            else
            {
                *existing = state;
            }
        }

        std::scoped_lock publish_lock{ tracking_publish_mutex_ };

        tracking_subject_
            .get_subscriber()
            .on_next(state);
    }

    std::vector<TrackingState>
        MessageQueue::tracking_snapshot() const
    {
        std::scoped_lock lock{ tracking_mutex_ };
        return tracking_states_;
    }

    rxcpp::observable<TrackingState>
        MessageQueue::tracking_changes() const
    {
        return tracking_subject_
            .get_observable()
            .as_dynamic();
    }

    void MessageQueue::register_communication(
        const xsim::aircrafts::CommunicationRequest& request)
    {
        auto registered = request;

        {
            std::scoped_lock lock{ communication_mutex_ };

            std::uint64_t next_sequence = 1;

            for (const auto& current : communication_records_)
            {
                if (::IsEqualGUID(
                        current.conversation_id,
                        registered.conversation_id))
                {
                    next_sequence =
                        (std::max)(
                            next_sequence,
                            current.sequence + 1);
                }
            }

            registered.sequence = next_sequence;
            communication_records_.push_back(registered);
        }

        std::scoped_lock publish_lock{ communication_publish_mutex_ };

        communication_subject_
            .get_subscriber()
            .on_next(registered);
    }

    std::vector<xsim::aircrafts::CommunicationRequest>
        MessageQueue::communication_snapshot() const
    {
        std::scoped_lock lock{ communication_mutex_ };
        return communication_records_;
    }

    rxcpp::observable<xsim::aircrafts::CommunicationRequest>
        MessageQueue::communication_changes() const
    {
        return communication_subject_
            .get_observable()
            .as_dynamic();
    }
}
