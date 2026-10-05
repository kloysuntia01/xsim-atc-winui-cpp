#include "pch.h"
#include "AircraftTrackingViewModel.h"

#if __has_include("AircraftTrackingViewModel.g.cpp")
#include "AircraftTrackingViewModel.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    AircraftTrackingViewModel::AircraftTrackingViewModel(
        winrt::hstring fake_guid,
        winrt::hstring call_sign,
        double x,
        double y)
        : fake_guid_(std::move(fake_guid)),
          call_sign_(std::move(call_sign)),
          x_(x),
          y_(y)
    {
    }

    winrt::hstring AircraftTrackingViewModel::FakeGuid() const
    {
        return fake_guid_;
    }

    winrt::hstring AircraftTrackingViewModel::CallSign() const
    {
        return call_sign_;
    }

    double AircraftTrackingViewModel::X() const noexcept
    {
        return x_;
    }

    double AircraftTrackingViewModel::Y() const noexcept
    {
        return y_;
    }

    void AircraftTrackingViewModel::Apply(double x, double y)
    {
        x_ = x;
        y_ = y;

        RaisePropertyChanged(L"X");
        RaisePropertyChanged(L"Y");
    }
}
