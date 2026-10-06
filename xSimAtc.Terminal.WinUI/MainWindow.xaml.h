#pragma once

#include "MainWindow.g.h"
#include "MainViewModel.h"
#include "TowerViewModel.h"
#include "message_queue.h"

#include <memory>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        MainWindow(
            xSimAtc_Terminal_WinUI::MainViewModel const& main_view_model,
            xSimAtc_Terminal_WinUI::TowerViewModel const& tower_view_model,
            std::shared_ptr<xsim::communications::MessageQueue> message_queue);

        void OnNavigationToggleClick(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

        void OnPinNavigationClick(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        void ApplyNavigationMode();
        void CloseNavigationFlyout();

        bool navigation_pinned_{ false };

        xSimAtc_Terminal_WinUI::MainViewModel main_view_model_{ nullptr };
        xSimAtc_Terminal_WinUI::TowerViewModel tower_view_model_{ nullptr };
        std::shared_ptr<xsim::communications::MessageQueue> message_queue_;
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct MainWindow :
        MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
