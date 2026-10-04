#include "pch.h"
#include "transponder.h"


namespace xsim::aircrafts
{
    using std::chrono::milliseconds;
    using std::move;
    using std::stop_token;

    Transponder::Transponder(
        const GUID& aircraft_id,
        std::string call_sign)
        : aircraft_id_(aircraft_id),
        call_sign_(move(call_sign))
    {
    }

    Transponder::~Transponder()
    {
        stop();
    }

    void Transponder::start(
        milliseconds interval)
    {
        stop();

        worker_ =
            std::jthread(
                [this, interval](
                    stop_token stop_token)
                {
                    run(
                        stop_token,
                        interval);
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
            subject_
                .get_subscriber()
                .on_next(
                    TransponderSignal{
                        aircraft_id_,
                        call_sign_,
                        std::chrono::
                            steady_clock::now()
                    });

            std::this_thread::
                sleep_for(interval);
        }
    }
}