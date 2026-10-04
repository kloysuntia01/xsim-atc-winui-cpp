#pragma once

#include "transponder_signal.h"

#include <chrono>
#include <guiddef.h>
#include <rxcpp/rx.hpp>
#include <string>
#include <thread>

namespace xsim::aircrafts
{
    class Transponder final
    {
    public:
        Transponder(
            const GUID& aircraft_id,
            std::string call_sign);

        ~Transponder();

        Transponder(
            const Transponder&) = delete;

        Transponder&
            operator=(
                const Transponder&) = delete;

        void start(
            std::chrono::milliseconds interval =
            std::chrono::seconds{ 1 });

        void stop();

        [[nodiscard]]
        rxcpp::observable<TransponderSignal>
            signals() const;

    private:
        void run(
            std::stop_token stop_token,
            std::chrono::milliseconds interval);

        GUID aircraft_id_{};

        std::string call_sign_;

        rxcpp::subjects::subject<
            TransponderSignal>
            subject_;

        std::jthread worker_;
    };
}
