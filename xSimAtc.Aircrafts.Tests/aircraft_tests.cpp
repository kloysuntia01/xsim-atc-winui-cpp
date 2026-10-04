#include "pch.h"

#include "aircraft.h"

#include <gtest/gtest.h>

namespace xsim::aircrafts::tests
{
    TEST(AircraftTests, ConstructorPreservesCallSign)
    {
        Aircraft aircraft{ "FDX606" };

        EXPECT_EQ(
            aircraft.call_sign(),
            "FDX606");
    }

    TEST(AircraftTests, ConstructorGeneratesNonEmptyGuid)
    {
        Aircraft aircraft{ "FDX606" };

        const GUID empty_guid{};

        EXPECT_NE(
            std::memcmp(
                &aircraft.id(),
                &empty_guid,
                sizeof(GUID)),
            0);
    }

    TEST(AircraftTests, TwoAircraftGenerateDifferentGuids)
    {
        Aircraft first{ "FDX606" };
        Aircraft second{ "DAL101" };

        EXPECT_NE(
            std::memcmp(
                &first.id(),
                &second.id(),
                sizeof(GUID)),
            0);
    }
}