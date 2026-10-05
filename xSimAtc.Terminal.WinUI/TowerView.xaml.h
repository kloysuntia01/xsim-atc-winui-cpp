#pragma once

#include "TowerView.g.h"
#include "TowerViewModel.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct TowerView : TowerViewT<TowerView>
    {
        TowerView();

        void OnSendClick(
            Windows::Foundation::IInspectable const&,
            Microsoft::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct TowerView :
        TowerViewT<TowerView, implementation::TowerView>
    {
    };
}
