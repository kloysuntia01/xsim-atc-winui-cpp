#include "pch.h"
#include "TowerView.xaml.h"

#if __has_include("TowerView.g.cpp")
#include "TowerView.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    TowerView::TowerView()
    {
        InitializeComponent();
    }

    void TowerView::OnSendClick(
        Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        DataContext()
            .as<xSimAtc_Terminal_WinUI::TowerViewModel>()
            .Send();
    }
}
