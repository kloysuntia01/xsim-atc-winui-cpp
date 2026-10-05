#pragma once

#include "Communication/communication_request.h"
#include "message_record.h"
#include "tracking_state.h"

#include <mutex>
#include <rxcpp/rx.hpp>
#include <vector>

namespace xsim::communications
{
    class MessageQueue final
    {
    public:
        void send(const MessageRecord& record);

        [[nodiscard]]
        rxcpp::observable<MessageRecord> records() const;

        void register_tracking(const TrackingState& state);

        [[nodiscard]]
        std::vector<TrackingState> tracking_snapshot() const;

        [[nodiscard]]
        rxcpp::observable<TrackingState> tracking_changes() const;

        void register_communication(
            const xsim::aircrafts::CommunicationRequest& request);

        [[nodiscard]]
        std::vector<xsim::aircrafts::CommunicationRequest>
            communication_snapshot() const;

        [[nodiscard]]
        rxcpp::observable<xsim::aircrafts::CommunicationRequest>
            communication_changes() const;

    private:
        rxcpp::subjects::subject<MessageRecord> subject_;
        rxcpp::subjects::subject<TrackingState> tracking_subject_;
        rxcpp::subjects::subject<xsim::aircrafts::CommunicationRequest>
            communication_subject_;

        mutable std::mutex tracking_mutex_;
        std::mutex tracking_publish_mutex_;
        std::vector<TrackingState> tracking_states_;

        mutable std::mutex communication_mutex_;
        std::mutex communication_publish_mutex_;
        std::vector<xsim::aircrafts::CommunicationRequest>
            communication_records_;
    };
}
