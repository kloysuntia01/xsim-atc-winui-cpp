#include "pch.h"
#include "TowerViewModel.h"

#if __has_include("TowerViewModel.g.cpp")
#include "TowerViewModel.g.cpp"
#endif

namespace
{
    winrt::hstring guid_to_hstring(GUID const& value)
    {
        wchar_t buffer[64]{};
        ::StringFromGUID2(
            value,
            buffer,
            static_cast<int>(std::size(buffer)));

        return winrt::hstring{ buffer };
    }
}

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    TowerViewModel::TowerViewModel(
        std::shared_ptr<xsim::communications::MessageQueue> message_queue,
        std::shared_ptr<xsim::advisory::IAtcAdvisoryService>
            advisory_service,
        std::shared_ptr<xsimatc::speech::ITextToSpeech>
            text_to_speech)
        : message_queue_(std::move(message_queue)),
          advisory_service_(std::move(advisory_service)),
          text_to_speech_(std::move(text_to_speech))
    {
        if (!advisory_service_)
        {
            throw std::invalid_argument(
                "TowerViewModel requires an ATC advisory service.");
        }

        if (!text_to_speech_)
        {
            throw std::invalid_argument(
                "TowerViewModel requires a text-to-speech service.");
        }

        message_queue_
            ->records()
            .subscribe(
                subscription_,
                [this](xsim::communications::MessageRecord const& record)
                {
                    on_message(record);
                });
    }

    TowerViewModel::~TowerViewModel()
    {
        subscription_.unsubscribe();
    }

    winrt::hstring TowerViewModel::TowerGuid() const
    {
        return guid_to_hstring(tower_id_);
    }

    winrt::hstring TowerViewModel::PendingMessage() const
    {
        return L"N123";
    }

    winrt::hstring TowerViewModel::LastMessage() const
    {
        return last_message_;
    }

    winrt::hstring TowerViewModel::AdvisoryStatus() const
    {
        return advisory_status_;
    }

    winrt::hstring TowerViewModel::AdvisoryText() const
    {
        return advisory_text_;
    }

    winrt::hstring TowerViewModel::AdvisoryMutationStatus() const
    {
        return advisory_mutation_status_;
    }

    void TowerViewModel::Send()
    {
        message_queue_->send(
            xsim::communications::MessageRecord{
                tower_id_,
                receiver_id_,
                "N123"
            });
    }

    void TowerViewModel::RequestAdvisory()
    {
        advisory_status_ = L"Requesting advisory...";
        RaisePropertyChanged(L"AdvisoryStatus");

        const xsim::advisory::AtcAdvisoryRequest request{
            .runway = "18R",
            .runway_status = "Occupied",
            .call_sign = "FDX606",
            .aircraft_status = "Holding Short"
        };

        const auto result =
            advisory_service_->advise(request);

        advisory_text_ =
            winrt::to_hstring(result.text);

        advisory_status_ =
            L"Advisory returned.";

        advisory_mutation_status_ =
            result.world_state_changed
                ? L"World state changed: YES"
                : L"World state changed: NO";

        RaisePropertyChanged(L"AdvisoryText");
        RaisePropertyChanged(L"AdvisoryStatus");
        RaisePropertyChanged(L"AdvisoryMutationStatus");

        // First Tower advisory speech slice:
        // use the same advisory text shown in the UI as the spoken text.
        // Speech stays behind ITextToSpeech; Tower knows nothing about SAPI.
        text_to_speech_->speak(result.text);
    }

    void TowerViewModel::on_message(
        xsim::communications::MessageRecord const& record)
    {
        last_message_ = winrt::to_hstring(record.message);
        RaisePropertyChanged(L"LastMessage");
    }
}
