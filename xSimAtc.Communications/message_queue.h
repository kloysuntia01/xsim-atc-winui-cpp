#pragma once

#include "message_record.h"

#include <rxcpp/rx.hpp>

namespace xsim::communications
{
    class MessageQueue final
    {
    public:
        void send(const MessageRecord& record);

        [[nodiscard]]
        rxcpp::observable<MessageRecord> records() const;

    private:
        rxcpp::subjects::subject<MessageRecord> subject_;
    };
}
