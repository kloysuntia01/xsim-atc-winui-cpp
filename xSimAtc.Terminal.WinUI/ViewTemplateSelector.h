#pragma once

#include "ViewTemplateSelector.g.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    struct ViewTemplateSelector :
        ViewTemplateSelectorT<ViewTemplateSelector>
    {
        ViewTemplateSelector() = default;

        Microsoft::UI::Xaml::DataTemplate TowerTemplate() const;
        void TowerTemplate(
            Microsoft::UI::Xaml::DataTemplate const& value);

        Microsoft::UI::Xaml::DataTemplate TrackingsTemplate() const;
        void TrackingsTemplate(
            Microsoft::UI::Xaml::DataTemplate const& value);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
            Windows::Foundation::IInspectable const& item);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
            Windows::Foundation::IInspectable const& item,
            Microsoft::UI::Xaml::DependencyObject const& container);

    private:
        Microsoft::UI::Xaml::DataTemplate tower_template_{ nullptr };
        Microsoft::UI::Xaml::DataTemplate trackings_template_{ nullptr };
    };
}

namespace winrt::xSimAtc_Terminal_WinUI::factory_implementation
{
    struct ViewTemplateSelector :
        ViewTemplateSelectorT<
            ViewTemplateSelector,
            implementation::ViewTemplateSelector>
    {
    };
}
