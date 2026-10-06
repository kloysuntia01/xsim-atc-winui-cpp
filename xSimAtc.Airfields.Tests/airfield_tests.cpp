#include "pch.h"

#include "airfield.h"

namespace xsim::airfields::tests
{
    TEST(AirfieldTests, AirfieldStartsEmpty)
    {
        Airfield airfield;

        EXPECT_TRUE(airfield.positions().empty());
        EXPECT_TRUE(airfield.runways().empty());
        EXPECT_TRUE(airfield.nodes().empty());
        EXPECT_TRUE(airfield.aircraft_markers().empty());
        EXPECT_TRUE(airfield.tower_markers().empty());
    }

    TEST(AirfieldTests, AirfieldCanAddRunway)
    {
        Airfield airfield;

        airfield.runways().add(
            Runway{
                "18L/36R",
                { 0.44, 0.10 },
                { 0.44, 0.90 }
            });

        ASSERT_EQ(airfield.runways().size(), 1u);
        EXPECT_EQ(airfield.runways().begin()->id, "18L/36R");
    }

    TEST(AirfieldTests, AirfieldCanAddNode)
    {
        Airfield airfield;

        airfield.nodes().add(
            AirfieldNode{
                "F",
                AirfieldNodeKind::TaxiPoint,
                { 0.70, 0.48 }
            });

        ASSERT_EQ(airfield.nodes().size(), 1u);
        EXPECT_EQ(airfield.nodes().begin()->id, "F");
    }

    TEST(AirfieldTests, AirfieldCanAddAircraftMarker)
    {
        Airfield airfield;

        airfield.aircraft_markers().add(
            AircraftMarker{
                "FDX606",
                { 0.70, 0.48 }
            });

        ASSERT_EQ(
            airfield.aircraft_markers().size(),
            1u);

        EXPECT_EQ(
            airfield.aircraft_markers().begin()->call_sign,
            "FDX606");
    }

    TEST(AirfieldTests, AirfieldCollectionsCanBeEnumerated)
    {
        Airfield airfield;

        airfield.runways().add(
            Runway{
                "18L/36R",
                { 0.44, 0.10 },
                { 0.44, 0.90 }
            });

        airfield.runways().add(
            Runway{
                "18R/36L",
                { 0.30, 0.10 },
                { 0.30, 0.90 }
            });

        std::size_t count = 0;

        for (const auto& runway : airfield.runways())
        {
            (void)runway;
            ++count;
        }

        EXPECT_EQ(count, 2u);
    }
}
