#include "pch.h"
#include "MainViewModel.h"

#if __has_include("ViewContainer.g.cpp")
#include "ViewContainer.g.cpp"
#endif

#if __has_include("MainViewModel.g.cpp")
#include "MainViewModel.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    ViewContainer::ViewContainer(
        winrt::hstring title,
        Windows::Foundation::IInspectable content)
        : title_(std::move(title)),
          content_(std::move(content))
    {
    }

    winrt::hstring ViewContainer::Title() const
    {
        return title_;
    }

    Windows::Foundation::IInspectable ViewContainer::Content() const
    {
        return content_;
    }

    winrt::hstring MainViewModel::Title() const
    {
        return L"xSimAtc";
    }

    xSimAtc_Terminal_WinUI::ViewContainer
    MainViewModel::SelectedView() const
    {
        return selected_view_;
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

    void MainViewModel::NavigateToTower()
    {
        navigate_to_tower_requested_(*this, nullptr);
    }

    void MainViewModel::NavigateToTrackings()
    {
        navigate_to_trackings_requested_(*this, nullptr);
    }

    void MainViewModel::SelectView(
        xSimAtc_Terminal_WinUI::ViewContainer const& view)
    {
        selected_view_ = view;
        RaisePropertyChanged(L"SelectedView");
    }
}
