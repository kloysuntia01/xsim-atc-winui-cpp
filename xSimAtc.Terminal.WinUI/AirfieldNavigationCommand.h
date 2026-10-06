#pragma once

#include <functional>

#include <winrt/Microsoft.UI.Xaml.Input.h>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct AirfieldNavigationCommand :
        winrt::implements<
            AirfieldNavigationCommand,
            Microsoft::UI::Xaml::Input::ICommand>
    {
        explicit AirfieldNavigationCommand(
            std::function<void()> execute)
            : execute_(std::move(execute))
        {
        }

        [[nodiscard]]
        bool CanExecute(
            Windows::Foundation::IInspectable const&) const
        {
            return true;
        }

        void Execute(
            Windows::Foundation::IInspectable const&)
        {
            execute_();
        }

        winrt::event_token CanExecuteChanged(
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable>
                    const& handler)
        {
            return can_execute_changed_.add(handler);
        }

        void CanExecuteChanged(
            winrt::event_token const& token)
        {
            can_execute_changed_.remove(token);
        }

    private:
        std::function<void()> execute_;

        winrt::event<
            Windows::Foundation::EventHandler<
                Windows::Foundation::IInspectable>>
                    can_execute_changed_;
    };
}
