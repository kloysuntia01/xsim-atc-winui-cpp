#include "pch.h"
#include "TrackingsView.xaml.h"

#if __has_include("TrackingsView.g.cpp")
#include "TrackingsView.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    TrackingsView::TrackingsView()
    {
        InitializeComponent();
    }
}
