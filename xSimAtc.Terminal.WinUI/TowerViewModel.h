#pragma once

#include "TowerViewModel.g.h"
#include "message_queue.h"
#include "ModelBase.h"

#include <memory>
#include <rxcpp/rx.hpp>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct TowerViewModel :
        TowerViewModelT<TowerViewModel>,
        xsim::terminal::models::ModelBase
    {
        explicit TowerViewModel(
            std::shared_ptr<xsim::communications::MessageQueue> message_queue);

        ~TowerViewModel();

        winrt::hstring TowerGuid() const;
        winrt::hstring PendingMessage() const;
        winrt::hstring LastMessage() const;

        void Send();

    private:
        void on_message(
            xsim::communications::MessageRecord const& record);

        std::shared_ptr<xsim::communications::MessageQueue> message_queue_;
        rxcpp::composite_subscription subscription_;

        GUID tower_id_{
            0x11111111, 0x2222, 0x3333,
            { 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb }
        };

        GUID receiver_id_{
            0xaaaaaaaa, 0xbbbb, 0xcccc,
            { 0xdd, 0xee, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 }
        };

        winrt::hstring last_message_{ L"(empty)" };
    };
}
