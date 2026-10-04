#include "pch.h"

#include "transponder.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <gtest/gtest.h>
#include <mutex>
#include <objbase.h>
#include <stdexcept>
#include <string>
#include <thread>

namespace xsim::aircrafts::tests
{
    using namespace std::chrono_literals;

    namespace
    {
        GUID make_guid()
        {
            GUID id{};

            const auto result = ::CoCreateGuid(&id);

            if (FAILED(result))
            {
                throw std::runtime_error(
                    "Unable to generate test GUID.");
            }

            return id;
        }

        bool same_guid(
            const GUID& left,
            const GUID& right) noexcept
        {
            return std::memcmp(
                &left,
                &right,
                sizeof(GUID)) == 0;
        }
    }

    TEST(
        TransponderTests,
        StartEmitsAtLeastOneSignal)
    {
        const auto aircraft_id = make_guid();

        Transponder transponder{
            aircraft_id,
            "FDX606"
        };

        std::mutex mutex;
        std::condition_variable signal_received;
        bool received = false;

        auto subscription =
            transponder
                .signals()
                .subscribe(
                    [&](const TransponderSignal&)
                    {
                        {
                            std::lock_guard lock{ mutex };
                            received = true;
                        }

                        signal_received.notify_one();
                    });

        transponder.start(25ms);

        {
            std::unique_lock lock{ mutex };

            EXPECT_TRUE(
                signal_received.wait_for(
                    lock,
                    500ms,
                    [&]
                    {
                        return received;
                    }));
        }

        transponder.stop();
        subscription.unsubscribe();
    }

    TEST(
        TransponderTests,
        SignalContainsCallSign)
    {
        const auto aircraft_id = make_guid();

        Transponder transponder{
            aircraft_id,
            "DAL101"
        };

        std::mutex mutex;
        std::condition_variable signal_received;
        std::string received_call_sign;

        auto subscription =
            transponder
                .signals()
                .subscribe(
                    [&](const TransponderSignal& signal)
                    {
                        {
                            std::lock_guard lock{ mutex };
                            received_call_sign =
                                signal.call_sign;
                        }

                        signal_received.notify_one();
                    });

        transponder.start(25ms);

        {
            std::unique_lock lock{ mutex };

            ASSERT_TRUE(
                signal_received.wait_for(
                    lock,
                    500ms,
                    [&]
                    {
                        return !received_call_sign.empty();
                    }));
        }

        transponder.stop();
        subscription.unsubscribe();

        EXPECT_EQ(
            received_call_sign,
            "DAL101");
    }

    TEST(
        TransponderTests,
        SignalContainsAircraftGuid)
    {
        const auto aircraft_id = make_guid();

        Transponder transponder{
            aircraft_id,
            "UAL201"
        };

        std::mutex mutex;
        std::condition_variable signal_received;
        GUID received_id{};
        bool received = false;

        auto subscription =
            transponder
                .signals()
                .subscribe(
                    [&](const TransponderSignal& signal)
                    {
                        {
                            std::lock_guard lock{ mutex };
                            received_id =
                                signal.aircraft_id;
                            received = true;
                        }

                        signal_received.notify_one();
                    });

        transponder.start(25ms);

        {
            std::unique_lock lock{ mutex };

            ASSERT_TRUE(
                signal_received.wait_for(
                    lock,
                    500ms,
                    [&]
                    {
                        return received;
                    }));
        }

        transponder.stop();
        subscription.unsubscribe();

        EXPECT_TRUE(
            same_guid(
                aircraft_id,
                received_id));
    }

    TEST(
        TransponderTests,
        EmitsMultipleSignals)
    {
        const auto aircraft_id = make_guid();

        Transponder transponder{
            aircraft_id,
            "SWA404"
        };

        std::condition_variable signals_received;
        std::mutex wait_mutex;
        std::atomic<int> count{ 0 };

        auto subscription =
            transponder
                .signals()
                .subscribe(
                    [&](const TransponderSignal&)
                    {
                        const auto current =
                            ++count;

                        if (current >= 3)
                        {
                            signals_received.notify_one();
                        }
                    });

        transponder.start(20ms);

        {
            std::unique_lock lock{ wait_mutex };

            EXPECT_TRUE(
                signals_received.wait_for(
                    lock,
                    500ms,
                    [&]
                    {
                        return count.load() >= 3;
                    }));
        }

        transponder.stop();
        subscription.unsubscribe();

        EXPECT_GE(
            count.load(),
            3);
    }

    TEST(
        TransponderTests,
        StopPreventsFutureSignals)
    {
        const auto aircraft_id = make_guid();

        Transponder transponder{
            aircraft_id,
            "AAL303"
        };

        std::atomic<int> count{ 0 };

        auto subscription =
            transponder
                .signals()
                .subscribe(
                    [&](const TransponderSignal&)
                    {
                        ++count;
                    });

        transponder.start(20ms);

        const auto deadline =
            std::chrono::steady_clock::now()
            + 500ms;

        while (
            count.load() < 2
            && std::chrono::steady_clock::now()
                < deadline)
        {
            std::this_thread::sleep_for(5ms);
        }

        ASSERT_GE(
            count.load(),
            2);

        transponder.stop();

        const auto count_after_stop =
            count.load();

        std::this_thread::sleep_for(75ms);

        subscription.unsubscribe();

        EXPECT_EQ(
            count.load(),
            count_after_stop);
    }
}
