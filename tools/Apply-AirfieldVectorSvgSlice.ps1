param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$ui = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI"
$proj = Join-Path $ui "xSimAtc.Terminal.WinUI.vcxproj"
$cpp  = Join-Path $ui "AirfieldView.xaml.cpp"

foreach ($path in @($proj, $cpp))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

# ------------------------------------------------------------
# Register SVG asset for packaging.
# ------------------------------------------------------------
$xml = Get-Content $proj -Raw

if ($xml -notmatch 'Assets\\Airfield\\airfield-layout\.svg')
{
    $item = @'
  <ItemGroup>
    <Content Include="Assets\Airfield\airfield-layout.svg">
      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
    </Content>
  </ItemGroup>
'@

    $xml = $xml -replace '</Project>', "$item`r`n</Project>"
    Set-Content $proj $xml -Encoding utf8
    Write-Host "Registered Assets\Airfield\airfield-layout.svg for packaging."
}
else
{
    Write-Host "SVG asset already registered."
}

# ------------------------------------------------------------
# Dynamic selected route becomes one vector Path.
# ------------------------------------------------------------
$cppText = Get-Content $cpp -Raw

if ($cppText -notmatch '#include <sstream>')
{
    if ($cppText -match '#include <string>')
    {
        $cppText = $cppText -replace `
            '#include <string>', `
            "#include <sstream>`r`n#include <string>"
    }
    else
    {
        $cppText = "#include <sstream>`r`n" + $cppText
    }
}

# Static runways are now rendered by the SVG background.
$cppText = $cppText -replace `
    '(?m)^\s*render_runways\(\);\s*$', `
    '        // Static runway/taxi geometry lives in airfield-layout.svg.'

$routePattern = '(?ms)void\s+AirfieldView::render_selected_route\(\)\s*\{.*?^\s*\}'

$routeReplacement = @'
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

        std::wostringstream data;

        data
            << L"M "
            << screen_x(positions.front().x)
            << L","
            << screen_y(positions.front().y);

        for (std::size_t index = 1u;
             index < positions.size();
             ++index)
        {
            data
                << L" L "
                << screen_x(positions[index].x)
                << L","
                << screen_y(positions[index].y);
        }

        Microsoft::UI::Xaml::Shapes::Path route_path;

        route_path.Data(
            Microsoft::UI::Xaml::Media::Geometry::Parse(
                data.str()));

        route_path.Stroke(
            brush(
                color(
                    255,
                    193,
                    7)));

        route_path.StrokeThickness(7.0);

        route_path.StrokeLineJoin(
            Microsoft::UI::Xaml::Media::
                PenLineJoin::Round);

        route_path.StrokeStartLineCap(
            Microsoft::UI::Xaml::Media::
                PenLineCap::Round);

        route_path.StrokeEndLineCap(
            Microsoft::UI::Xaml::Media::
                PenLineCap::Round);

        AirfieldCanvas().Children().Append(
            route_path);
    }
'@

$matches = [regex]::Matches($cppText, $routePattern)

if ($matches.Count -eq 1)
{
    $cppText = [regex]::Replace(
        $cppText,
        $routePattern,
        $routeReplacement)

    Write-Host "Replaced selected route segments with one SVG-style Path geometry."
}
elseif ($cppText -match 'std::wostringstream data;')
{
    Write-Host "Vector route Path already applied."
}
else
{
    throw "Could not safely locate render_selected_route() for replacement."
}

Set-Content $cpp $cppText -Encoding utf8

Write-Host ""
Write-Host "Option B vector Airfield slice applied."
Write-Host ""
Write-Host "Static SVG:"
Write-Host "  runways"
Write-Host "  taxiway network"
Write-Host "  apron"
Write-Host "  terminal"
Write-Host "  gates / parked aircraft"
Write-Host ""
Write-Host "Live overlay remains:"
Write-Host "  selected route Path"
Write-Host "  clickable nodes"
Write-Host "  aircraft marker"
Write-Host "  tower marker"
Write-Host ""
Write-Host "Test baseline should remain 60/60."
