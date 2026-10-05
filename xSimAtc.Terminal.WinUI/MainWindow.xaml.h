#pragma once

#include "MainWindow.g.h"
#include "MainViewModel.h"
#include "TowerViewModel.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        MainWindow(
            xSimAtc_Terminal_WinUI::MainViewModel const& main_view_model,
            xSimAtc_Terminal_WinUI::TowerViewModel const& tower_view_model);

        void OnTowerClick(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        xSimAtc_Terminal_WinUI::MainViewModel main_view_model_{ nullptr };
        xSimAtc_Terminal_WinUI::TowerViewModel tower_view_model_{ nullptr };
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct MainWindow :
        MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
