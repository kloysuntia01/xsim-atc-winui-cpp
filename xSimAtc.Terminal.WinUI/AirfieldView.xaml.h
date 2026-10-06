#pragma once

#include "AirfieldView.g.h"
#include "AirfieldViewModel.h"
#include "AirfieldTaxiUiRuntime.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct AirfieldView :
        AirfieldViewT<AirfieldView>
    {
        AirfieldView();

    private:
        void render_airfield();
        void render_runways();
        void render_selected_route();
        void render_nodes();
        void render_towers();
        void render_aircraft();
        void render_selection_status();

        void on_aircraft_clicked(
            std::string call_sign);

        void on_node_clicked(
            std::string node_id);

        void on_runway_clicked(
            std::string runway);

        void on_clear_for_takeoff_clicked();

        void on_taxi_tick();

        [[nodiscard]]
        static double screen_x(
            double normalized) noexcept;

        [[nodiscard]]
        static double screen_y(
            double normalized) noexcept;

        winrt::xSimAtc_Terminal_WinUI::AirfieldViewModel
            view_model_{ nullptr };

        ::xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime
            taxi_ui_runtime_;

        Microsoft::UI::Xaml::DispatcherTimer
            taxi_timer_{ nullptr };

        std::string last_clearance_;
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct AirfieldView :
        AirfieldViewT<
            AirfieldView,
            implementation::AirfieldView>
    {
};
}





