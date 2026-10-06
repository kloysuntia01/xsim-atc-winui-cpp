#include "pch.h"

#include "aircraft_clock.h"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

namespace xsim::aircrafts::tests
{
    using namespace std::chrono_literals;

    TEST(AircraftClockTests, AircraftClockStartsStopped)
    {
        AircraftClock clock{ "Taxi" };

        EXPECT_FALSE(clock.is_running());
        EXPECT_EQ(clock.elapsed(), 0ms);
    }

    TEST(AircraftClockTests, AircraftClockCanStart)
    {
        AircraftClock clock{ "Taxi" };

        clock.start();

        EXPECT_TRUE(clock.is_running());
    }

    TEST(AircraftClockTests, AircraftClockElapsedIncreasesWhileRunning)
    {
        AircraftClock clock{ "Taxi" };

        clock.start();
        std::this_thread::sleep_for(15ms);

        EXPECT_GE(clock.elapsed(), 10ms);
    }

    TEST(AircraftClockTests, AircraftClockCanStop)
    {
        AircraftClock clock{ "Taxi" };

        clock.start();
        std::this_thread::sleep_for(15ms);
        clock.stop();

        const auto stopped_elapsed = clock.elapsed();

        std::this_thread::sleep_for(15ms);

        EXPECT_FALSE(clock.is_running());
        EXPECT_EQ(clock.elapsed(), stopped_elapsed);
        EXPECT_GE(stopped_elapsed, 10ms);
    }

    TEST(AircraftClockTests, AircraftClockCanReset)
    {
        AircraftClock clock{ "Taxi" };

        clock.start();
        std::this_thread::sleep_for(15ms);
        clock.reset();

        EXPECT_FALSE(clock.is_running());
        EXPECT_EQ(clock.elapsed(), 0ms);
    }

    TEST(AircraftClockTests, TwoAircraftClocksRunIndependently)
    {
        AircraftClock taxi{ "Taxi" };
        AircraftClock holding{ "Holding" };

        taxi.start();
        std::this_thread::sleep_for(15ms);

        holding.start();
        std::this_thread::sleep_for(15ms);

        taxi.stop();

        const auto taxi_elapsed = taxi.elapsed();
        const auto holding_elapsed_before = holding.elapsed();

        std::this_thread::sleep_for(15ms);

        EXPECT_FALSE(taxi.is_running());
        EXPECT_TRUE(holding.is_running());
        EXPECT_EQ(taxi.elapsed(), taxi_elapsed);
        EXPECT_GT(holding.elapsed(), holding_elapsed_before);
        EXPECT_GT(taxi_elapsed, holding_elapsed_before);
    }
}
