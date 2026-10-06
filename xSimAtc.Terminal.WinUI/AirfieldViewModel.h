#pragma once

#include "AirfieldViewModel.g.h"

#include "../xSimAtc.Airfields/airfield.h"
#include "../xSimAtc.Airfields/airfield_layout.h"
#include "../xSimAtc.Airfields/Routing/airfield_route_selection.h"

#include <string>
#include <string_view>
#include <vector>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct AirfieldViewModel :
        AirfieldViewModelT<AirfieldViewModel>
    {
        AirfieldViewModel();

        [[nodiscard]]
        const xsim::airfields::Airfield&
        airfield() const noexcept;

        void select_aircraft(std::string call_sign);

        [[nodiscard]]
        const std::string&
        selected_aircraft_call_sign() const noexcept;

        [[nodiscard]]
        bool has_selected_aircraft() const noexcept;

        bool select_route_node(std::string node_id);

        [[nodiscard]]
        bool is_route_node_selected(
            std::string_view node_id) const;

        [[nodiscard]]
        const std::vector<std::string>&
        selected_route_node_ids() const noexcept;

        [[nodiscard]]
        std::vector<xsim::airfields::AirfieldPosition>
        selected_route_positions() const;

        void clear_route() noexcept;

    private:
        void seed_airfield();

        xsim::airfields::Airfield airfield_;
        xsim::airfields::AirfieldRouteSelection route_selection_;
        std::string selected_aircraft_call_sign_;
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct AirfieldViewModel :
        AirfieldViewModelT<
            AirfieldViewModel,
            implementation::AirfieldViewModel>
    {
    };
}
