#pragma once

#include "ViewContainer.g.h"
#include "MainViewModel.g.h"
#include "ModelBase.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct ViewContainer : ViewContainerT<ViewContainer>
    {
        ViewContainer(
            winrt::hstring title,
            Windows::Foundation::IInspectable content);

        winrt::hstring Title() const;
        Windows::Foundation::IInspectable Content() const;

    private:
        winrt::hstring title_;
        Windows::Foundation::IInspectable content_{ nullptr };
    };

    struct MainViewModel :
        MainViewModelT<MainViewModel>,
        xsim::terminal::models::ModelBase
    {
        MainViewModel() = default;

        winrt::hstring Title() const;

        xSimAtc_Terminal_WinUI::ViewContainer SelectedView() const;

        winrt::event_token NavigateToTowerRequested(
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable> const& handler);

        void NavigateToTowerRequested(
            winrt::event_token const& token) noexcept;

        winrt::event_token NavigateToTrackingsRequested(
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable> const& handler);

        void NavigateToTrackingsRequested(
            winrt::event_token const& token) noexcept;

        void NavigateToTower();
        void NavigateToTrackings();

        void SelectView(
            xSimAtc_Terminal_WinUI::ViewContainer const& view);

    private:
        xSimAtc_Terminal_WinUI::ViewContainer selected_view_{ nullptr };

        winrt::event<
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable>>
            navigate_to_tower_requested_;

        winrt::event<
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable>>
            navigate_to_trackings_requested_;
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct MainViewModel :
        MainViewModelT<MainViewModel, implementation::MainViewModel>
    {
    };
}
