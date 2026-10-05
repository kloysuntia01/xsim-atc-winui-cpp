#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "MainViewModel.h"
#include "TowerViewModel.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    App::App()
        : message_queue_(
            std::make_shared<xsim::communications::MessageQueue>())
    {
    }

    void App::OnLaunched(
        Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        auto main_view_model =
            winrt::make<MainViewModel>();

        auto tower_view_model =
            winrt::make<TowerViewModel>(message_queue_);

        window_ = winrt::make<MainWindow>(
            main_view_model,
            tower_view_model);

        window_.Activate();
    }
}
