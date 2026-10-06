#include "pch.h"
#include "MainWindow.xaml.h"
#include "TrackingsViewModel.h"

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        ApplyNavigationMode();
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

        ApplyNavigationMode();

        main_view_model_.NavigateToTowerRequested(
            [this](
                Windows::Foundation::IInspectable const&,
                Windows::Foundation::IInspectable const&)
            {
                CloseNavigationFlyout();
                main_view_model_.SelectViewModel(tower_view_model_);
            });

        main_view_model_.NavigateToTrackingsRequested(
            [this](
                Windows::Foundation::IInspectable const&,
                Windows::Foundation::IInspectable const&)
            {
                CloseNavigationFlyout();

                // Trackings remains view-scoped. Each navigation creates a
                // fresh VM that hydrates from Msg.Q and subscribes to Rx.
                auto trackings_view_model =
                    winrt::make<TrackingsViewModel>(message_queue_);

                main_view_model_.SelectViewModel(trackings_view_model);
            });

        // Initial route.
        main_view_model_.SelectViewModel(tower_view_model_);
    }

    void MainWindow::OnNavigationToggleClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        if (navigation_pinned_)
        {
            return;
        }

        NavigationSplitView().IsPaneOpen(
            !NavigationSplitView().IsPaneOpen());
    }

    void MainWindow::OnPinNavigationClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        navigation_pinned_ = !navigation_pinned_;
        ApplyNavigationMode();
    }

    void MainWindow::ApplyNavigationMode()
    {
        if (navigation_pinned_)
        {
            NavigationSplitView().DisplayMode(
                Microsoft::UI::Xaml::Controls::SplitViewDisplayMode::CompactInline);
            NavigationSplitView().IsPaneOpen(true);
            PinNavigationIcon().Glyph(L"\uE77A");
            Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(
                PinNavigationButton(),
                winrt::box_value(L"Unpin navigation"));
            return;
        }

        NavigationSplitView().DisplayMode(
            Microsoft::UI::Xaml::Controls::SplitViewDisplayMode::CompactOverlay);
        NavigationSplitView().IsPaneOpen(false);
        PinNavigationIcon().Glyph(L"\uE718");
        Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(
            PinNavigationButton(),
            winrt::box_value(L"Pin navigation"));
    }

    void MainWindow::CloseNavigationFlyout()
    {
        if (!navigation_pinned_)
        {
            NavigationSplitView().IsPaneOpen(false);
        }
    }
}
