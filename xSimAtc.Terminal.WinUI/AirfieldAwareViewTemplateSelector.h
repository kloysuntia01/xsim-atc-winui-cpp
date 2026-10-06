#pragma once

#include "AirfieldAwareViewTemplateSelector.g.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct AirfieldAwareViewTemplateSelector :
        AirfieldAwareViewTemplateSelectorT<
            AirfieldAwareViewTemplateSelector>
    {
        AirfieldAwareViewTemplateSelector() = default;

        Microsoft::UI::Xaml::DataTemplate
        TowerTemplate() const noexcept;

        void TowerTemplate(
            Microsoft::UI::Xaml::DataTemplate const& value) noexcept;

        Microsoft::UI::Xaml::DataTemplate
        TrackingsTemplate() const noexcept;

        void TrackingsTemplate(
            Microsoft::UI::Xaml::DataTemplate const& value) noexcept;

        Microsoft::UI::Xaml::DataTemplate
        AirfieldTemplate() const noexcept;

        void AirfieldTemplate(
            Microsoft::UI::Xaml::DataTemplate const& value) noexcept;

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
            Windows::Foundation::IInspectable const& item);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
            Windows::Foundation::IInspectable const& item,
            Microsoft::UI::Xaml::DependencyObject const& container);

    private:
        Microsoft::UI::Xaml::DataTemplate
            tower_template_{ nullptr };

        Microsoft::UI::Xaml::DataTemplate
            trackings_template_{ nullptr };

        Microsoft::UI::Xaml::DataTemplate
            airfield_template_{ nullptr };
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct AirfieldAwareViewTemplateSelector :
        AirfieldAwareViewTemplateSelectorT<
            AirfieldAwareViewTemplateSelector,
            implementation::AirfieldAwareViewTemplateSelector>
    {
    };
}
