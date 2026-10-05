#include "pch.h"
#include "MainWindow.xaml.h"
#include "TowerView.xaml.h"

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
        xSimAtc_Terminal_WinUI::TowerViewModel const& tower_view_model)
        : main_view_model_(main_view_model),
          tower_view_model_(tower_view_model)
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
                auto tower_view =
                    winrt::make<TowerView>();

                tower_view
                    .as<Microsoft::UI::Xaml::FrameworkElement>()
                    .DataContext(tower_view_model_);

                auto container =
                    winrt::make<ViewContainer>(
                        L"Tower",
                        tower_view);

                main_view_model_.SelectView(container);
            });
    }

    void MainWindow::OnTowerClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        main_view_model_.NavigateToTower();
    }
}
