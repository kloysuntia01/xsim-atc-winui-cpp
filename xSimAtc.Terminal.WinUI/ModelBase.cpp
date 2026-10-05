#include "pch.h"
#include "ModelBase.h"

namespace xsim::terminal::models
{
    ModelBase::~ModelBase() = default;

    winrt::event_token ModelBase::PropertyChanged(
        PropertyChangedEventHandler const& handler)
    {
        return property_changed_.add(handler);
    }

    void ModelBase::PropertyChanged(
        winrt::event_token const& token) noexcept
    {
        property_changed_.remove(token);
    }

    void ModelBase::RaisePropertyChanged(
        winrt::hstring const& property_name)
    {
        property_changed_(
            nullptr,
            winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{
                property_name
            });
    }
}
