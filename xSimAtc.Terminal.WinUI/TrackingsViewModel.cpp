#include "pch.h"
#include "TrackingsViewModel.h"

#include <objbase.h>

#if __has_include("TrackingsViewModel.g.cpp")
#include "TrackingsViewModel.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    TrackingsViewModel::TrackingsViewModel(
        std::shared_ptr<xsim::communications::MessageQueue> message_queue)
        : message_queue_(std::move(message_queue)),
          dispatcher_queue_(
              Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread())
    {
        ::OutputDebugStringW(L"TrackingsViewModel constructed\n");

        LoadSnapshot();
        Subscribe();
    }

    TrackingsViewModel::~TrackingsViewModel()
    {
        subscription_.unsubscribe();
        ::OutputDebugStringW(L"TrackingsViewModel destroyed\n");
    }

    winrt::hstring TrackingsViewModel::Title() const
    {
        return L"Trackings";
    }

    winrt::hstring TrackingsViewModel::Status() const
    {
        return L"Msg.Q tracking state | 5 long-lived aircraft | Rx live";
    }

    Windows::Foundation::Collections::IVector<
        Windows::Foundation::IInspectable>
    TrackingsViewModel::Aircraft() const
    {
        return aircraft_;
    }

    void TrackingsViewModel::LoadSnapshot()
    {
        for (const auto& state : message_queue_->tracking_snapshot())
        {
            AddOrUpdate(state);
        }
    }

    void TrackingsViewModel::Subscribe()
    {
        auto weak_this = get_weak();

        message_queue_
            ->tracking_changes()
            .subscribe(
                subscription_,
                [weak_this](
                    const xsim::communications::TrackingState& state)
                {
                    if (auto strong = weak_this.get())
                    {
                        const auto queue = strong->dispatcher_queue_;

                        queue.TryEnqueue(
                            [weak_this, state]()
                            {
                                if (auto current = weak_this.get())
                                {
                                    current->AddOrUpdate(state);
                                }
                            });
                    }
                });
    }

    void TrackingsViewModel::AddOrUpdate(
        const xsim::communications::TrackingState& state)
    {
        const auto guid = GuidToString(state.aircraft_id);

        for (std::uint32_t index = 0; index < aircraft_.Size(); ++index)
        {
            auto vm =
                aircraft_
                    .GetAt(index)
                    .as<xSimAtc_Terminal_WinUI::AircraftTrackingViewModel>();

            if (vm.FakeGuid() == guid)
            {
                winrt::get_self<AircraftTrackingViewModel>(vm)
                    ->Apply(state.x, state.y);
                return;
            }
        }

        auto vm = winrt::make<AircraftTrackingViewModel>(
            guid,
            winrt::to_hstring(state.call_sign),
            state.x,
            state.y);

        aircraft_.Append(vm);
    }

    winrt::hstring TrackingsViewModel::GuidToString(const GUID& id)
    {
        wchar_t buffer[39]{};
        ::StringFromGUID2(id, buffer, 39);
        return buffer;
    }
}
