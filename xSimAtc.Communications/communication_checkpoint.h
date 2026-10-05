#pragma once

#include "communication_record.h"

#include <guiddef.h>
#include <rxcpp/rx.hpp>

namespace xsim::communications
{
    class CommunicationCheckpoint final
    {
    public:
        void send(
            const GUID& sender_id,
            const GUID& receiver_id);

        [[nodiscard]]
        rxcpp::observable<CommunicationRecord> records() const;

    private:
        rxcpp::subjects::subject<CommunicationRecord> subject_;
    };
}
