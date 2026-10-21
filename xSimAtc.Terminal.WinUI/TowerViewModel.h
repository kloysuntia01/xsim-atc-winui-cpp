#pragma once

#include "TowerViewModel.g.h"
#include "message_queue.h"
#include "ModelBase.h"

#include "../xSimAtc.Advisory/i_atc_advisory_service.h"
#include "../xSimAtc.Speech/i_text_to_speech.h"

#include <memory>
#include <rxcpp/rx.hpp>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct TowerViewModel :
        TowerViewModelT<TowerViewModel>,
        xsim::terminal::models::ModelBase
    {
        TowerViewModel(
            std::shared_ptr<xsim::communications::MessageQueue> message_queue,
            std::shared_ptr<xsim::advisory::IAtcAdvisoryService>
                advisory_service,
            std::shared_ptr<xsimatc::speech::ITextToSpeech>
                text_to_speech);

        ~TowerViewModel();

        winrt::hstring TowerGuid() const;
        winrt::hstring PendingMessage() const;
        winrt::hstring LastMessage() const;

        winrt::hstring AdvisoryStatus() const;
        winrt::hstring AdvisoryText() const;
        winrt::hstring AdvisoryMutationStatus() const;

        void Send();
        void RequestAdvisory();

    private:
        void on_message(
            xsim::communications::MessageRecord const& record);

        std::shared_ptr<xsim::communications::MessageQueue> message_queue_;
        std::shared_ptr<xsim::advisory::IAtcAdvisoryService>
            advisory_service_;
        std::shared_ptr<xsimatc::speech::ITextToSpeech>
            text_to_speech_;

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

        winrt::hstring advisory_status_{
            L"Advisory not requested."
        };

        winrt::hstring advisory_text_{};

        winrt::hstring advisory_mutation_status_{
            L"World state changed: NO"
        };
    };
}
