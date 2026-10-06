#include <gtest/gtest.h>

#include "../xSimAtc.Airfields/airfield_taxi_runtime.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTaxiRuntimeCompletionHookTests, CompletionCallbackFiresOnce)
    {
        AirfieldTaxiRuntime runtime;

        int completions = 0;

        runtime.set_completed_callback(
            [&completions]()
            {
                ++completions;
            });

        ASSERT_TRUE(
            runtime.load_and_start({
                "F",
                "H"
            }));

        runtime.advance(10.0);
        runtime.advance(10.0);

        EXPECT_TRUE(runtime.is_completed());
        EXPECT_EQ(completions, 1);
    }

    TEST(AirfieldTaxiRuntimeCompletionHookTests, NewRouteRearmsCompletionCallback)
    {
        AirfieldTaxiRuntime runtime;

        int completions = 0;

        runtime.set_completed_callback(
            [&completions]()
            {
                ++completions;
            });

        ASSERT_TRUE(
            runtime.load_and_start({
                "F",
                "H"
            }));

        runtime.advance(10.0);

        ASSERT_TRUE(
            runtime.load_and_start({
                "H",
                "J"
            }));

        runtime.advance(10.0);

        EXPECT_EQ(completions, 2);
    }
}
