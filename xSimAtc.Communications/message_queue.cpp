#include "pch.h"
#include "message_queue.h"

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
}
