#pragma once

namespace xsim::terminal::models
{
    // Abstract reusable base for WinUI bindable model/view-model implementations.
    //
    // The WinRT interface contract is declared in IDL:
    //
    //   Microsoft.UI.Xaml.Data.INotifyPropertyChanged
    //
    // ModelBase centralizes the reusable C++ event implementation.
    class ModelBase
    {
    public:
        using PropertyChangedEventHandler =
            winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler;

        ModelBase(ModelBase const&) = delete;
        ModelBase& operator=(ModelBase const&) = delete;

        virtual ~ModelBase() = 0;

        winrt::event_token PropertyChanged(
            PropertyChangedEventHandler const& handler);

        void PropertyChanged(
            winrt::event_token const& token) noexcept;

    protected:
        ModelBase() = default;

        void RaisePropertyChanged(
            winrt::hstring const& property_name);

    private:
        winrt::event<PropertyChangedEventHandler> property_changed_;
    };
}
