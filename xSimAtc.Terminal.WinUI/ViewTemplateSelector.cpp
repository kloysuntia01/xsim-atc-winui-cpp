#include "pch.h"
#include "ViewTemplateSelector.h"

#if __has_include("ViewTemplateSelector.g.cpp")
#include "ViewTemplateSelector.g.cpp"
#endif

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    Microsoft::UI::Xaml::DataTemplate
    ViewTemplateSelector::TowerTemplate() const
    {
        return tower_template_;
    }

    void ViewTemplateSelector::TowerTemplate(
        Microsoft::UI::Xaml::DataTemplate const& value)
    {
        tower_template_ = value;
    }

    Microsoft::UI::Xaml::DataTemplate
    ViewTemplateSelector::TrackingsTemplate() const
    {
        return trackings_template_;
    }

    void ViewTemplateSelector::TrackingsTemplate(
        Microsoft::UI::Xaml::DataTemplate const& value)
    {
        trackings_template_ = value;
    }

    Microsoft::UI::Xaml::DataTemplate
    ViewTemplateSelector::SelectTemplateCore(
        Windows::Foundation::IInspectable const& item)
    {
        if (item.try_as<xSimAtc_Terminal_WinUI::TowerViewModel>())
        {
            return tower_template_;
        }

        if (item.try_as<xSimAtc_Terminal_WinUI::TrackingsViewModel>())
        {
            return trackings_template_;
        }

        return nullptr;
    }

    Microsoft::UI::Xaml::DataTemplate
    ViewTemplateSelector::SelectTemplateCore(
        Windows::Foundation::IInspectable const& item,
        Microsoft::UI::Xaml::DependencyObject const&)
    {
        return SelectTemplateCore(item);
    }
}
