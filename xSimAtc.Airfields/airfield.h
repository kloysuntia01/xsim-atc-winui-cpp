#pragma once

#include "Collections/collection.h"
#include "Markers/aircraft_marker.h"
#include "Markers/tower_marker.h"
#include "Nodes/airfield_node.h"
#include "Positions/airfield_position.h"
#include "Runways/runway.h"

namespace xsim::airfields
{
    class Airfield final
    {
    public:
        [[nodiscard]]
        AirfieldPositions& positions() noexcept
        {
            return positions_;
        }

        [[nodiscard]]
        const AirfieldPositions& positions() const noexcept
        {
            return positions_;
        }

        [[nodiscard]]
        Runways& runways() noexcept
        {
            return runways_;
        }

        [[nodiscard]]
        const Runways& runways() const noexcept
        {
            return runways_;
        }

        [[nodiscard]]
        AirfieldNodes& nodes() noexcept
        {
            return nodes_;
        }

        [[nodiscard]]
        const AirfieldNodes& nodes() const noexcept
        {
            return nodes_;
        }

        [[nodiscard]]
        AircraftMarkers& aircraft_markers() noexcept
        {
            return aircraft_markers_;
        }

        [[nodiscard]]
        const AircraftMarkers& aircraft_markers() const noexcept
        {
            return aircraft_markers_;
        }

        [[nodiscard]]
        TowerMarkers& tower_markers() noexcept
        {
            return tower_markers_;
        }

        [[nodiscard]]
        const TowerMarkers& tower_markers() const noexcept
        {
            return tower_markers_;
        }

    private:
        AirfieldPositions positions_;
        Runways runways_;
        AirfieldNodes nodes_;
        AircraftMarkers aircraft_markers_;
        TowerMarkers tower_markers_;
    };
}
