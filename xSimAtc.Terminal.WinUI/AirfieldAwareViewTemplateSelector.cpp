#include "pch.h"

#include "AirfieldAwareViewTemplateSelector.h"
#include "AirfieldAwareViewTemplateSelector.g.cpp"

#include "AirfieldViewModel.h"
#include "TowerViewModel.h"
#include "TrackingsViewModel.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    Microsoft::UI::Xaml::DataTemplate
    AirfieldAwareViewTemplateSelector::TowerTemplate()
        const noexcept
    {
        return tower_template_;
    }

    void AirfieldAwareViewTemplateSelector::TowerTemplate(
        Microsoft::UI::Xaml::DataTemplate const& value) noexcept
    {
        tower_template_ = value;
    }

    Microsoft::UI::Xaml::DataTemplate
    AirfieldAwareViewTemplateSelector::TrackingsTemplate()
        const noexcept
    {
        return trackings_template_;
    }

    void AirfieldAwareViewTemplateSelector::TrackingsTemplate(
        Microsoft::UI::Xaml::DataTemplate const& value) noexcept
    {
        trackings_template_ = value;
    }

    Microsoft::UI::Xaml::DataTemplate
    AirfieldAwareViewTemplateSelector::AirfieldTemplate()
        const noexcept
    {
        return airfield_template_;
    }

    void AirfieldAwareViewTemplateSelector::AirfieldTemplate(
        Microsoft::UI::Xaml::DataTemplate const& value) noexcept
    {
        airfield_template_ = value;
    }

    Microsoft::UI::Xaml::DataTemplate
    AirfieldAwareViewTemplateSelector::SelectTemplateCore(
        Windows::Foundation::IInspectable const& item)
    {
        if (item.try_as<
                winrt::xSimAtc_Terminal_WinUI::
                    AirfieldViewModel>())
        {
            return airfield_template_;
        }

        if (item.try_as<
                winrt::xSimAtc_Terminal_WinUI::
                    TowerViewModel>())
        {
            return tower_template_;
        }

        if (item.try_as<
                winrt::xSimAtc_Terminal_WinUI::
                    TrackingsViewModel>())
        {
            return trackings_template_;
        }

        return nullptr;
    }

    Microsoft::UI::Xaml::DataTemplate
    AirfieldAwareViewTemplateSelector::SelectTemplateCore(
        Windows::Foundation::IInspectable const& item,
        Microsoft::UI::Xaml::DependencyObject const&)
    {
        return SelectTemplateCore(item);
    }
}
