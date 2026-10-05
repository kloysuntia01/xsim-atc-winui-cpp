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
        std::shared_ptr<xsim::communications::MessageQueue> message_queue)
        : message_queue_(std::move(message_queue))
    {
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

    void TowerViewModel::Send()
    {
        message_queue_->send(
            xsim::communications::MessageRecord{
                tower_id_,
                receiver_id_,
                "N123"
            });
    }

    void TowerViewModel::on_message(
        xsim::communications::MessageRecord const& record)
    {
        last_message_ = winrt::to_hstring(record.message);
        RaisePropertyChanged(L"LastMessage");
    }
}
