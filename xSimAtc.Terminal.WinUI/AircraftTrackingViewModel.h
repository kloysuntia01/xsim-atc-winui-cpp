#pragma once

#include "AircraftTrackingViewModel.g.h"
#include "ModelBase.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct AircraftTrackingViewModel :
        AircraftTrackingViewModelT<AircraftTrackingViewModel>,
        xsim::terminal::models::ModelBase
    {
        AircraftTrackingViewModel(
            winrt::hstring fake_guid,
            winrt::hstring call_sign,
            double x,
            double y);

        winrt::hstring FakeGuid() const;
        winrt::hstring CallSign() const;
        double X() const noexcept;
        double Y() const noexcept;

        void Apply(double x, double y);

    private:
        winrt::hstring fake_guid_;
        winrt::hstring call_sign_;
        double x_{};
        double y_{};
    };
}
