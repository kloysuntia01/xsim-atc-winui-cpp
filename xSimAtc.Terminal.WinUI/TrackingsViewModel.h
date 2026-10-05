#pragma once

#include "TrackingsViewModel.g.h"
#include "AircraftTrackingViewModel.h"
#include "ModelBase.h"
#include "message_queue.h"

#include <memory>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct TrackingsViewModel :
        TrackingsViewModelT<TrackingsViewModel>,
        xsim::terminal::models::ModelBase
    {
        explicit TrackingsViewModel(
            std::shared_ptr<xsim::communications::MessageQueue> message_queue);

        ~TrackingsViewModel();

        winrt::hstring Title() const;
        winrt::hstring Status() const;

        Windows::Foundation::Collections::IVector<
            Windows::Foundation::IInspectable>
        Aircraft() const;

    private:
        void LoadSnapshot();
        void Subscribe();
        void AddOrUpdate(
            const xsim::communications::TrackingState& state);

        static winrt::hstring GuidToString(const GUID& id);

        std::shared_ptr<xsim::communications::MessageQueue> message_queue_;
        rxcpp::composite_subscription subscription_;

        Microsoft::UI::Dispatching::DispatcherQueue dispatcher_queue_{ nullptr };

        Windows::Foundation::Collections::IVector<
            Windows::Foundation::IInspectable>
            aircraft_{
                winrt::single_threaded_observable_vector<
                    Windows::Foundation::IInspectable>()
            };
    };
}
