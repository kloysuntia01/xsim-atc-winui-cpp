param(
    [string]$RepoRoot = "C:\\Dev\\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$cpp = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\\AirfieldView.xaml.cpp"

if (-not (Test-Path $cpp))
{
    throw "Missing file: $cpp"
}

$backup = "$cpp.before-vector-route-repair"

if (-not (Test-Path $backup))
{
    Copy-Item $cpp $backup
}

$text = Get-Content $cpp -Raw

$signature = "void AirfieldView::render_selected_route()"
$start = $text.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find render_selected_route()."
}

$open = $text.IndexOf("{", $start)

if ($open -lt 0)
{
    throw "Could not find opening brace for render_selected_route()."
}

$depth = 0
$close = -1

for ($i = $open; $i -lt $text.Length; $i++)
{
    if ($text[$i] -eq '{') { $depth++ }
    elseif ($text[$i] -eq '}')
    {
        $depth--
        if ($depth -eq 0)
        {
            $close = $i
            break
        }
    }
}

if ($close -lt 0)
{
    throw "Could not find closing brace for render_selected_route()."
}

$replacement = @'
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
                    screen_x(positions.front().x)),
                static_cast<float>(
                    screen_y(positions.front().y))
            });

        for (std::size_t index = 1u;
             index < positions.size();
             ++index)
        {
            Microsoft::UI::Xaml::Media::LineSegment segment;

            segment.Point(
                Windows::Foundation::Point{
                    static_cast<float>(
                        screen_x(positions[index].x)),
                    static_cast<float>(
                        screen_y(positions[index].y))
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
'@

$text =
    $text.Substring(0, $start) +
    $replacement +
    $text.Substring($close + 1)

$text = [regex]::Replace(
    $text,
    '(?m)^\s*#include\s+<sstream>\s*\r?\n?',
    '')

Set-Content $cpp $text -Encoding utf8

Write-Host ""
Write-Host "Vector route renderer repaired."
Write-Host "Expected test baseline: 60/60."
