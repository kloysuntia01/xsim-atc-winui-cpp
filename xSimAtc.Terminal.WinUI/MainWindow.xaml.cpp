#include "pch.h"
#include "MainWindow.xaml.h"
#include "TowerView.xaml.h"
#include "TrackingsView.xaml.h"
#include "TrackingsViewModel.h"

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    MainWindow::MainWindow(
        xSimAtc_Terminal_WinUI::MainViewModel const& main_view_model,
        xSimAtc_Terminal_WinUI::TowerViewModel const& tower_view_model,
        std::shared_ptr<xsim::communications::MessageQueue> message_queue)
        : main_view_model_(main_view_model),
          tower_view_model_(tower_view_model),
          message_queue_(std::move(message_queue))
    {
        InitializeComponent();

        Content()
            .as<Microsoft::UI::Xaml::FrameworkElement>()
            .DataContext(main_view_model_);

        main_view_model_.NavigateToTowerRequested(
            [this](
                Windows::Foundation::IInspectable const&,
                Windows::Foundation::IInspectable const&)
            {
                auto tower_view = winrt::make<TowerView>();

                tower_view
                    .as<Microsoft::UI::Xaml::FrameworkElement>()
                    .DataContext(tower_view_model_);

                auto container =
                    winrt::make<ViewContainer>(L"Tower", tower_view);

                main_view_model_.SelectView(container);
            });

        main_view_model_.NavigateToTrackingsRequested(
            [this](
                Windows::Foundation::IInspectable const&,
                Windows::Foundation::IInspectable const&)
            {
                // Trackings is view-scoped. Every visit gets a fresh VM that
                // hydrates from Msg.Q and then subscribes to live Rx changes.
                auto trackings_view_model =
                    winrt::make<TrackingsViewModel>(message_queue_);

                auto trackings_view = winrt::make<TrackingsView>();

                trackings_view
                    .as<Microsoft::UI::Xaml::FrameworkElement>()
                    .DataContext(trackings_view_model);

                auto container =
                    winrt::make<ViewContainer>(L"Trackings", trackings_view);

                main_view_model_.SelectView(container);
            });
    }

    void MainWindow::OnTowerClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        main_view_model_.NavigateToTower();
    }

    void MainWindow::OnTrackingsClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        main_view_model_.NavigateToTrackings();
    }
}
