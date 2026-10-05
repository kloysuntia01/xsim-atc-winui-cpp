#pragma once

#include "TrackingsView.g.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct TrackingsView : TrackingsViewT<TrackingsView>
    {
        TrackingsView();
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct TrackingsView :
        TrackingsViewT<TrackingsView, implementation::TrackingsView>
    {
    };
}
