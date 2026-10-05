#pragma once

#include "App.xaml.g.h"
#include "message_queue.h"

#include <memory>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(
            Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        std::shared_ptr<xsim::communications::MessageQueue> message_queue_;
        Microsoft::UI::Xaml::Window window_{ nullptr };
    };
}
