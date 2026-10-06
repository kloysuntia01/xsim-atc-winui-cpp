#include "pch.h"
#include "AirfieldViewModel.h"
#include "AirfieldViewModel.g.cpp"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    AirfieldViewModel::AirfieldViewModel()
    {
        seed_airfield();
    }

    const xsim::airfields::Airfield&
    AirfieldViewModel::airfield() const noexcept
    {
        return airfield_;
    }

    void AirfieldViewModel::select_aircraft(
        std::string call_sign)
    {
        selected_aircraft_call_sign_ =
            std::move(call_sign);

        selected_runway_.clear();
        route_selection_.clear();
    }

    const std::string&
    AirfieldViewModel::selected_aircraft_call_sign()
        const noexcept
    {
        return selected_aircraft_call_sign_;
    }

    bool AirfieldViewModel::has_selected_aircraft()
        const noexcept
    {
        return !selected_aircraft_call_sign_.empty();
    }

    void AirfieldViewModel::assign_runway(
        std::string runway)
    {
        if (!has_selected_aircraft())
        {
            return;
        }

        selected_runway_ =
            std::move(runway);
    }

    const std::string&
    AirfieldViewModel::selected_runway() const noexcept
    {
        return selected_runway_;
    }

    bool AirfieldViewModel::has_selected_runway()
        const noexcept
    {
        return !selected_runway_.empty();
    }

    bool AirfieldViewModel::select_route_node(
        std::string node_id)
    {
        if (!has_selected_aircraft())
        {
            return false;
        }

        return route_selection_.add(
            std::move(node_id));
    }

    bool AirfieldViewModel::is_route_node_selected(
        std::string_view node_id) const
    {
        return route_selection_.contains(node_id);
    }

    const std::vector<std::string>&
    AirfieldViewModel::selected_route_node_ids()
        const noexcept
    {
        return route_selection_.node_ids();
    }

    std::vector<xsim::airfields::AirfieldPosition>
    AirfieldViewModel::selected_route_positions() const
    {
        std::vector<
            xsim::airfields::AirfieldPosition> positions;

        for (const auto& selected_id :
             route_selection_.node_ids())
        {
            for (const auto& node :
                 airfield_.nodes())
            {
                if (node.id == selected_id)
                {
                    positions.push_back(
                        node.position);

                    break;
                }
            }
        }

        return positions;
    }

    void AirfieldViewModel::clear_route() noexcept
    {
        route_selection_.clear();
    }

    void AirfieldViewModel::seed_airfield()
    {
        using namespace xsim::airfields;

        airfield_.runways().add(
            Runway{
                "18R/36L",
                { 0.30, 0.10 },
                { 0.30, 0.90 }
            });

        airfield_.runways().add(
            Runway{
                "18L/36R",
                { 0.44, 0.10 },
                { 0.44, 0.90 }
            });

        airfield_.nodes().add(layout::F);
        airfield_.nodes().add(layout::H);
        airfield_.nodes().add(layout::J);
        airfield_.nodes().add(layout::Runway18L);

        airfield_.positions().add({ 0.30, 0.10 });
        airfield_.positions().add({ 0.30, 0.90 });
        airfield_.positions().add({ 0.44, 0.10 });
        airfield_.positions().add({ 0.44, 0.90 });

        for (const auto& node : airfield_.nodes())
        {
            airfield_.positions().add(
                node.position);
        }

        airfield_.tower_markers().add(
            TowerMarker{
                "Tower",
                { 0.70, 0.60 }
            });

        airfield_.aircraft_markers().add(
            AircraftMarker{
                "FDX606",
                layout::F.position
            });
    }
}
