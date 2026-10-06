#include "pch.h"
#include "MainViewModel.h"
#include "RelayCommand.h"

#if __has_include("MainViewModel.g.cpp")
#include "MainViewModel.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    MainViewModel::MainViewModel()
    {
        navigate_to_tower_command_ =
            winrt::make<xsim::terminal::commands::RelayCommand>(
                [this](Windows::Foundation::IInspectable const&)
                {
                    navigate_to_tower_requested_(*this, nullptr);
                });

        navigate_to_trackings_command_ =
            winrt::make<xsim::terminal::commands::RelayCommand>(
                [this](Windows::Foundation::IInspectable const&)
                {
                    navigate_to_trackings_requested_(*this, nullptr);
                });
    }

    Windows::Foundation::IInspectable
    MainViewModel::SelectedViewModel() const
    {
        return selected_view_model_;
    }

    Microsoft::UI::Xaml::Input::ICommand
    MainViewModel::NavigateToTowerCommand() const
    {
        return navigate_to_tower_command_;
    }

    Microsoft::UI::Xaml::Input::ICommand
    MainViewModel::NavigateToTrackingsCommand() const
    {
        return navigate_to_trackings_command_;
    }

    winrt::event_token MainViewModel::NavigateToTowerRequested(
        Windows::Foundation::EventHandler<
            Windows::Foundation::IInspectable> const& handler)
    {
        return navigate_to_tower_requested_.add(handler);
    }

    void MainViewModel::NavigateToTowerRequested(
        winrt::event_token const& token) noexcept
    {
        navigate_to_tower_requested_.remove(token);
    }

    winrt::event_token MainViewModel::NavigateToTrackingsRequested(
        Windows::Foundation::EventHandler<
            Windows::Foundation::IInspectable> const& handler)
    {
        return navigate_to_trackings_requested_.add(handler);
    }

    void MainViewModel::NavigateToTrackingsRequested(
        winrt::event_token const& token) noexcept
    {
        navigate_to_trackings_requested_.remove(token);
    }

    void MainViewModel::SelectViewModel(
        Windows::Foundation::IInspectable const& view_model)
    {
        selected_view_model_ = view_model;
        RaisePropertyChanged(L"SelectedViewModel");
    }
}
