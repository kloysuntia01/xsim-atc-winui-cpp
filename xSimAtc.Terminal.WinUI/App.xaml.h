#pragma once

#include "App.xaml.g.h"
#include "aircraft.h"
#include "aircraft_mediator.h"
#include "message_queue.h"

#include <array>
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
        std::shared_ptr<xsim::communications::AircraftMediator> aircraft_mediator_;
        std::array<std::unique_ptr<xsim::aircrafts::Aircraft>, 5> aircraft_;

        Microsoft::UI::Xaml::Window window_{ nullptr };
    };
}
