#include "pch.h"

#include "MainViewModel.h"
#include "AirfieldNavigationCommand.h"
#include "AirfieldViewModel.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    Microsoft::UI::Xaml::Input::ICommand
    MainViewModel::NavigateToAirfieldCommand()
    {
        if (!navigate_to_airfield_command_)
        {
            navigate_to_airfield_command_ =
                winrt::make<AirfieldNavigationCommand>(
                    [this]()
                    {
                        selected_view_model_ =
                            winrt::make<AirfieldViewModel>();

                        RaisePropertyChanged(
                            L"SelectedViewModel");
                    });
        }

        return navigate_to_airfield_command_;
    }
}
