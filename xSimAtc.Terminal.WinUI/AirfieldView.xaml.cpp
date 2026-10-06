#include "pch.h"
#include "AirfieldView.xaml.h"

#if __has_include("AirfieldView.g.cpp")
#include "AirfieldView.g.cpp"
#endif

#include <cstdint>
#include <string>

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include "../xSimAtc.Airfields/airfield_taxiway_overlay.h"
#include "../xSimAtc.Airfields/airfield_taxiway_selection.h"
#include <chrono>

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    namespace
    {
        constexpr double canvas_width = 1000.0;
        constexpr double canvas_height = 700.0;

        Microsoft::UI::Xaml::Media::SolidColorBrush brush(
            Windows::UI::Color color)
        {
            return Microsoft::UI::Xaml::Media::SolidColorBrush{
                color
            };
        }

        Windows::UI::Color color(
            std::uint8_t r,
            std::uint8_t g,
            std::uint8_t b,
            std::uint8_t a = 255) noexcept
        {
            return Windows::UI::Color{
                a,
                r,
                g,
                b
            };
        }

        Microsoft::UI::Xaml::Thickness thickness(
            double value) noexcept
        {
            return Microsoft::UI::Xaml::Thickness{
                value,
                value,
                value,
                value
            };
        }
    }

    AirfieldView::AirfieldView()
    {
        InitializeComponent();

        view_model_ =
            winrt::make<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>();

        render_airfield();
    
        // FDX606_TAXI_TIMER_INIT
        taxi_timer_ =
            Microsoft::UI::Xaml::DispatcherTimer();

        taxi_timer_.Interval(
            std::chrono::milliseconds{ 50 });

        taxi_timer_.Tick(
            [this](auto&&, auto&&)
            {
                on_taxi_tick();
            });
}

        void AirfieldView::on_taxi_tick()
    {
        auto& runtime =
            taxi_ui_runtime_.taxi_runtime();

        if (!runtime.is_running())
        {
            taxi_timer_.Stop();
            return;
        }

        runtime.advance(0.04);

        render_airfield();

        if (!runtime.is_running())
        {
            taxi_timer_.Stop();
        }
    }

double AirfieldView::screen_x(
        double normalized) noexcept
    {
        return normalized * canvas_width;
    }

    double AirfieldView::screen_y(
        double normalized) noexcept
    {
        return normalized * canvas_height;
    }

    void AirfieldView::render_airfield()
    {
        AirfieldCanvas().Children().Clear();
        // Static runway/taxi geometry lives in airfield-layout.svg.
        render_selected_route();
        render_nodes();
        render_towers();
        render_aircraft();
        render_selection_status();
    }

    void AirfieldView::render_runways()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto& airfield = vm->airfield();

        for (const auto& runway :
             airfield.runways())
        {
            Microsoft::UI::Xaml::Shapes::Line line;

            line.X1(
                screen_x(
                    runway.north_end.x));

            line.Y1(
                screen_y(
                    runway.north_end.y));

            line.X2(
                screen_x(
                    runway.south_end.x));

            line.Y2(
                screen_y(
                    runway.south_end.y));

            line.Stroke(
                brush(
                    color(
                        68,
                        72,
                        72)));

            line.StrokeThickness(34.0);

            AirfieldCanvas().Children().Append(
                line);

            Microsoft::UI::Xaml::Shapes::Line center_line;

            center_line.X1(
                screen_x(
                    runway.north_end.x));

            center_line.Y1(
                screen_y(
                    runway.north_end.y));

            center_line.X2(
                screen_x(
                    runway.south_end.x));

            center_line.Y2(
                screen_y(
                    runway.south_end.y));

            center_line.Stroke(
                brush(
                    color(
                        235,
                        235,
                        220)));

            center_line.StrokeThickness(2.0);
            center_line.StrokeDashArray().Append(8.0);
            center_line.StrokeDashArray().Append(10.0);

            AirfieldCanvas().Children().Append(
                center_line);

            Microsoft::UI::Xaml::Controls::TextBlock label;

            label.Text(
                winrt::to_hstring(
                    runway.id));

            label.Foreground(
                brush(
                    color(
                        255,
                        255,
                        255)));

            label.FontSize(15.0);

            label.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    label,
                    screen_x(
                        runway.north_end.x) -
                        35.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    label,
                    screen_y(
                        runway.north_end.y) -
                        35.0);

            AirfieldCanvas().Children().Append(
                label);
        }
    }

    void AirfieldView::render_selected_route()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto positions =
            vm->selected_route_positions();

        if (positions.size() < 2u)
        {
            return;
        }

        Microsoft::UI::Xaml::Media::PathGeometry geometry;
        Microsoft::UI::Xaml::Media::PathFigure figure;

        figure.StartPoint(
            Windows::Foundation::Point{
                static_cast<float>(
                    screen_x(
                        positions.front().x)),
                static_cast<float>(
                    screen_y(
                        positions.front().y))
            });

        for (std::size_t index = 1u;
             index < positions.size();
             ++index)
        {
            Microsoft::UI::Xaml::Media::LineSegment segment;

            segment.Point(
                Windows::Foundation::Point{
                    static_cast<float>(
                        screen_x(
                            positions[index].x)),
                    static_cast<float>(
                        screen_y(
                            positions[index].y))
                });

            figure.Segments().Append(segment);
        }

        geometry.Figures().Append(figure);

        Microsoft::UI::Xaml::Shapes::Path route_path;

        route_path.Data(geometry);

        route_path.Stroke(
            brush(
                color(
                    255,
                    193,
                    7)));

        route_path.StrokeThickness(7.0);

        AirfieldCanvas().Children().Append(
            route_path);
    }
void AirfieldView::render_nodes()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto& airfield =
            vm->airfield();

        // OPTION_B_INTERACTIVE_TAXIWAY_PANELS
        //
        // Render physical/intermediate taxiway nodes first so the larger
        // controller decision nodes appended below remain on top.
        for (const auto* taxiway_node :
             xsim::airfields::taxiway_overlay::intermediate_nodes())
        {
            Microsoft::UI::Xaml::Controls::Button panel;

            panel.Width(30.0);
            panel.Height(22.0);
            panel.Padding(thickness(0.0));

            panel.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{
                    4.0
                });

            const bool selected =
                vm->is_route_node_selected(
                    std::string{
                        taxiway_node->id
                    });

            panel.Background(
                brush(
                    selected
                        ? color(
                            210,
                            255,
                            133,
                            27)
                        : color(
                            90,
                            14,
                            20,
                            16)));

            panel.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            232,
                            140)
                        : color(
                            190,
                            255,
                            193,
                            7)));

            panel.BorderThickness(
                thickness(
                    selected
                        ? 2.0
                        : 1.0));

            panel.Foreground(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255,
                            255)
                        : color(
                            230,
                            255,
                            232,
                            140)));

            panel.FontSize(10.0);

            panel.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            panel.Content(
                winrt::box_value(
                    winrt::to_hstring(
                        taxiway_node->id)));

            const auto node_id =
                std::string{
                    taxiway_node->id
                };

            panel.Click(
                [this, node_id](
                    auto&&,
                    auto&&)
                {
                    on_node_clicked(
                        node_id);
                });

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    panel,
                    screen_x(
                        taxiway_node->position.x) -
                        15.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    panel,
                    screen_y(
                        taxiway_node->position.y) -
                        11.0);

            AirfieldCanvas().Children().Append(
                panel);
        }

        // OPTION_B_CONTROLLER_NODE_REPAIR
        //
        // F / H / J / 18L are ATC decision nodes. They disappeared when
        // render_nodes() was reduced to intermediate-node loops only.
        // Render them last, larger, and slightly offset from nearby
        // physical waypoint labels. Domain coordinates are unchanged.
        for (const auto& node :
             airfield.nodes())
        {
            Microsoft::UI::Xaml::Controls::Button button;

            button.Width(52.0);
            button.Height(34.0);
            button.Padding(thickness(0.0));

            button.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{
                    6.0
                });

            const bool selected =
                vm->is_route_node_selected(
                    node.id);

            button.Background(
                brush(
                    selected
                        ? color(
                            225,
                            255,
                            133,
                            27)
                        : color(
                            125,
                            18,
                            22,
                            18)));

            button.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            245,
                            180)
                        : color(
                            235,
                            255,
                            193,
                            7)));

            button.BorderThickness(
                thickness(
                    selected
                        ? 3.0
                        : 2.0));

            button.Foreground(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255,
                            255)
                        : color(
                            255,
                            255,
                            240,
                            180)));

            button.FontSize(12.0);

            button.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            button.Content(
                winrt::box_value(
                    winrt::to_hstring(
                        node.id)));

            const auto node_id =
                node.id;

            button.Click(
                [this, node_id](
                    auto&&,
                    auto&&)
                {
                    on_node_clicked(
                        node_id);
                });

            double label_offset_x =
                -26.0;

            double label_offset_y =
                -17.0;

            if (node.id == "F")
            {
                label_offset_x = -26.0;
                label_offset_y = 12.0;
            }
            else if (node.id == "H")
            {
                label_offset_x = -26.0;
                label_offset_y = -48.0;
            }
            else if (node.id == "J")
            {
                label_offset_x = -62.0;
                label_offset_y = -8.0;
            }
            else if (node.id == "18L")
            {
                label_offset_x = -62.0;
                label_offset_y = -8.0;
            }

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    button,
                    screen_x(
                        node.position.x) +
                        label_offset_x);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    button,
                    screen_y(
                        node.position.y) +
                        label_offset_y);

            AirfieldCanvas().Children().Append(
                button);
        }
    }

    void AirfieldView::render_towers()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto& airfield = vm->airfield();

        for (const auto& tower :
             airfield.tower_markers())
        {
            Microsoft::UI::Xaml::Shapes::Ellipse marker;

            marker.Width(44.0);
            marker.Height(44.0);

            marker.Fill(
                brush(
                    color(
                        64,
                        130,
                        188)));

            marker.Stroke(
                brush(
                    color(
                        230,
                        240,
                        250)));

            marker.StrokeThickness(3.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    marker,
                    screen_x(
                        tower.position.x) -
                        22.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    marker,
                    screen_y(
                        tower.position.y) -
                        22.0);

            AirfieldCanvas().Children().Append(
                marker);

            Microsoft::UI::Xaml::Controls::TextBlock label;

            label.Text(
                winrt::to_hstring(
                    tower.id));

            label.Foreground(
                brush(
                    color(
                        255,
                        255,
                        255)));

            label.FontSize(14.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    label,
                    screen_x(
                        tower.position.x) +
                        28.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    label,
                    screen_y(
                        tower.position.y) -
                        10.0);

            AirfieldCanvas().Children().Append(
                label);
        }
    }

    void AirfieldView::render_aircraft()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto& airfield = vm->airfield();

        for (const auto& aircraft :
             airfield.aircraft_markers())
        {
        // FDX606_RUNTIME_MARKER_POSITION
        auto render_position =
            aircraft.position;

        if (
            aircraft.call_sign == "FDX606" &&
            taxi_ui_runtime_
                .taxi_runtime()
                .selected_nodes()
                .size() >= 2 &&
            (
                taxi_ui_runtime_.taxi_runtime().is_running() ||
                taxi_ui_runtime_.taxi_runtime().is_completed()
            )
        )
        {
            render_position =
                taxi_ui_runtime_
                    .taxi_runtime()
                    .position();
        }

            Microsoft::UI::Xaml::Controls::Button button;

            button.Width(118.0);
            button.Height(38.0);
            button.Padding(thickness(5.0));

            const bool selected =
                vm->selected_aircraft_call_sign() ==
                aircraft.call_sign;

            button.Background(
                brush(
                    selected
                        ? color(
                            77,
                            65,
                            16,
                            235)
                        : color(
                            20,
                            28,
                            20,
                            220)));

            button.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            193,
                            7)
                        : color(
                            90,
                            100,
                            90)));

            button.BorderThickness(
                thickness(
                    selected
                        ? 3.0
                        : 1.0));

            button.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{
                    8.0,
                    8.0,
                    8.0,
                    8.0
                });

            Microsoft::UI::Xaml::Controls::StackPanel content;
            content.Orientation(
                Microsoft::UI::Xaml::Controls::
                    Orientation::Horizontal);

            content.Spacing(6.0);

            Microsoft::UI::Xaml::Shapes::Rectangle marker;

            marker.Width(18.0);
            marker.Height(18.0);
            marker.RadiusX(4.0);
            marker.RadiusY(4.0);

            marker.Fill(
                brush(
                    color(
                        230,
                        92,
                        69)));

            marker.Stroke(
                brush(
                    color(
                        255,
                        255,
                        255)));

            marker.StrokeThickness(2.0);

            Microsoft::UI::Xaml::Controls::TextBlock label;

            label.Text(
                winrt::to_hstring(
                    aircraft.call_sign));

            label.Foreground(
                brush(
                    color(
                        255,
                        255,
                        255)));

            label.FontSize(13.0);

            label.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            label.VerticalAlignment(
                Microsoft::UI::Xaml::
                    VerticalAlignment::Center);

            content.Children().Append(
                marker);

            content.Children().Append(
                label);

            button.Content(content);

            const auto call_sign =
                aircraft.call_sign;

            button.Click(
                [this, call_sign](
                    auto&&,
                    auto&&)
                {
                    on_aircraft_clicked(
                        call_sign);
                });

            // Keep the aircraft control close to its actual
            // airfield position while leaving node F clickable.
            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    button,
                    screen_x(
                        render_position.x) +
                        22.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    button,
                    screen_y(
                        render_position.y) -
                        48.0);

            AirfieldCanvas().Children().Append(
                button);
        }
    }

    void AirfieldView::render_selection_status()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        Microsoft::UI::Xaml::Controls::Border card;

        card.Background(
            brush(
                color(
                    16,
                    23,
                    16,
                    225)));

        card.CornerRadius(
            Microsoft::UI::Xaml::CornerRadius{
                8.0,
                8.0,
                8.0,
                8.0
            });

        card.Padding(
            Microsoft::UI::Xaml::Thickness{
                12.0,
                8.0,
                12.0,
                8.0
            });

        Microsoft::UI::Xaml::Controls::StackPanel stack;
        stack.Spacing(3.0);

        Microsoft::UI::Xaml::Controls::TextBlock aircraft_text;

        aircraft_text.Foreground(
            brush(
                color(
                    255,
                    255,
                    255)));

        aircraft_text.FontSize(13.0);

        if (vm->has_selected_aircraft())
        {
            aircraft_text.Text(
                L"Aircraft: " +
                winrt::to_hstring(
                    vm->selected_aircraft_call_sign()));
        }
        else
        {
            aircraft_text.Text(
                L"Aircraft: click FDX606");
        }

        Microsoft::UI::Xaml::Controls::TextBlock route_text;

        route_text.Foreground(
            brush(
                color(
                    255,
                    214,
                    92)));

        route_text.FontSize(13.0);

        std::wstring route =
            L"Route: ";

        const auto& ids =
            vm->selected_route_node_ids();

        if (ids.empty())
        {
            route +=
                vm->has_selected_aircraft()
                    ? L"click F / H / J / 18L"
                    : L"select aircraft first";
        }
        else
        {
            for (std::size_t index = 0;
                 index < ids.size();
                 ++index)
            {
                if (index > 0)
                {
                    route += L" \u2192 ";
                }

                route +=
                    winrt::to_hstring(
                        ids[index]);
            }
        }

        route_text.Text(route);

        stack.Children().Append(
            aircraft_text);

        stack.Children().Append(
            route_text);

        card.Child(stack);

        Microsoft::UI::Xaml::Controls::Canvas::
            SetLeft(
                card,
                735.0);

        Microsoft::UI::Xaml::Controls::Canvas::
            SetTop(
                card,
                22.0);

        AirfieldCanvas().Children().Append(
            card);
    }

    void AirfieldView::on_aircraft_clicked(
        std::string call_sign)
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        vm->select_aircraft(
            std::move(call_sign));

        render_airfield();
    }

    void AirfieldView::on_node_clicked(
        std::string node_id)
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        vm->select_route_node(
            std::move(node_id));
        // FDX606_TAXI_RUNTIME_BINDING
        const auto& selected_nodes =
            vm->selected_route_node_ids();

        if (selected_nodes.size() >= 2)
        {
            const auto started =
                taxi_ui_runtime_
                    .taxi_runtime()
                    .load_and_start(
                        std::vector<std::string>{
                            selected_nodes.begin(),
                            selected_nodes.end()
                        });

            // FDX606_TAXI_TIMER_START
            if (started)
            {
                taxi_timer_.Start();
            }
        }


        render_airfield();
    }
}












