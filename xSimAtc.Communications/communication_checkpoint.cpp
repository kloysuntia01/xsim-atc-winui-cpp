#include "pch.h"
#include "communication_checkpoint.h"

namespace xsim::communications
{
    void CommunicationCheckpoint::send(
        const GUID& sender_id,
        const GUID& receiver_id)
    {
        subject_
            .get_subscriber()
            .on_next(
                CommunicationRecord{
                    sender_id,
                    receiver_id
                });
    }

    rxcpp::observable<CommunicationRecord>
        CommunicationCheckpoint::records() const
    {
        return subject_
            .get_observable()
            .as_dynamic();
    }
}
