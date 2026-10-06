#pragma once

#include <functional>

#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.Foundation.h>

namespace xsim::terminal::commands
{
    struct RelayCommand :
        winrt::implements<
            RelayCommand,
            winrt::Microsoft::UI::Xaml::Input::ICommand>
    {
        using ExecuteHandler =
            std::function<void(
                winrt::Windows::Foundation::IInspectable const&)>;

        explicit RelayCommand(ExecuteHandler execute)
            : execute_(std::move(execute))
        {
        }

        bool CanExecute(
            winrt::Windows::Foundation::IInspectable const&) const noexcept
        {
            return static_cast<bool>(execute_);
        }

        void Execute(
            winrt::Windows::Foundation::IInspectable const& parameter)
        {
            if (execute_)
            {
                execute_(parameter);
            }
        }

        winrt::event_token CanExecuteChanged(
            winrt::Windows::Foundation::EventHandler<
                winrt::Windows::Foundation::IInspectable> const& handler)
        {
            return can_execute_changed_.add(handler);
        }

        void CanExecuteChanged(
            winrt::event_token const& token) noexcept
        {
            can_execute_changed_.remove(token);
        }

    private:
        ExecuteHandler execute_;

        winrt::event<
            winrt::Windows::Foundation::EventHandler<
                winrt::Windows::Foundation::IInspectable>>
            can_execute_changed_;
    };
}
