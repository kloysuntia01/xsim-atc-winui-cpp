param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsCmake = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"
$view = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $testsCmake))
{
    throw "Missing test CMake file: $testsCmake"
}

if (-not (Test-Path $view))
{
    throw "Missing AirfieldView.xaml.cpp: $view"
}

# ------------------------------------------------------------
# 1) Register the two additive domain tests.
# ------------------------------------------------------------

$cmakeText = Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_taxiway_overlay_tests\.cpp')
{
    $append = @'

# Option-B intermediate taxiway overlay nodes
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_overlay_tests.cpp
)
'@

    Add-Content -Path $testsCmake -Value $append -Encoding utf8
    Write-Host "Registered airfield_taxiway_overlay_tests.cpp."
}
else
{
    Write-Host "airfield_taxiway_overlay_tests.cpp is already registered."
}

# ------------------------------------------------------------
# 2) Patch render_nodes() only.
#    We use exact source shapes already observed in the user's repo.
# ------------------------------------------------------------

$text = Get-Content $view -Raw

$includeLine = '#include "../xSimAtc.Airfields/airfield_taxiway_overlay.h"'

if ($text -notmatch [regex]::Escape($includeLine))
{
    $lastInclude = [regex]::Matches($text, '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    if (-not $lastInclude)
    {
        throw "Could not locate include block in AirfieldView.xaml.cpp"
    }

    $insertAt = $lastInclude.Index + $lastInclude.Length
    $text = $text.Insert($insertAt, "`r`n$includeLine")
}

$signature = "void AirfieldView::render_nodes()"
$start = $text.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::render_nodes()."
}

$openBrace = $text.IndexOf("{", $start)
if ($openBrace -lt 0)
{
    throw "Could not find render_nodes() opening brace."
}

$depth = 0
$closeBrace = -1

for ($i = $openBrace; $i -lt $text.Length; $i++)
{
    if ($text[$i] -eq '{')
    {
        $depth++
    }
    elseif ($text[$i] -eq '}')
    {
        $depth--

        if ($depth -eq 0)
        {
            $closeBrace = $i
            break
        }
    }
}

if ($closeBrace -lt 0)
{
    throw "Could not find render_nodes() closing brace."
}

$functionText = $text.Substring(
    $start,
    $closeBrace - $start + 1)

if ($functionText -match 'OPTION_B_INTERMEDIATE_TAXIWAY_PANELS')
{
    Write-Host "Intermediate taxiway panels already present."
    Write-Host "Expected test total remains 74."
    exit 0
}

# Guard against unexpected source drift.
if ($functionText -notmatch 'Microsoft::UI::Xaml::Controls::Button\s+button\s*;')
{
    throw "Expected existing controller-node Button declaration not found. No UI changes made."
}

# Insert before the function's final closing brace.
$overlayBlock = @'

        // OPTION_B_INTERMEDIATE_TAXIWAY_PANELS
        //
        // Physical taxiway-routing nodes are intentionally rendered as
        // non-interactive alpha panels. F/H/J/18L remain the controller
        // decision buttons; F1/H1/J1 expose the actual graph geometry.
        for (const auto* taxiway_node :
             xsim::airfields::taxiway_overlay::intermediate_nodes())
        {
            Microsoft::UI::Xaml::Controls::Border panel;

            panel.Width(30.0);
            panel.Height(22.0);
            panel.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{ 4.0 });

            panel.Background(
                brush(
                    color(
                        90,
                        14,
                        20,
                        16)));

            panel.BorderBrush(
                brush(
                    color(
                        190,
                        255,
                        193,
                        7)));

            panel.BorderThickness(
                thickness(1.0));

            Microsoft::UI::Xaml::Controls::TextBlock label;

            label.Text(
                winrt::to_hstring(
                    taxiway_node->id));

            label.Foreground(
                brush(
                    color(
                        230,
                        255,
                        232,
                        140)));

            label.FontSize(10.0);

            label.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            label.HorizontalAlignment(
                Microsoft::UI::Xaml::
                    HorizontalAlignment::Center);

            label.VerticalAlignment(
                Microsoft::UI::Xaml::
                    VerticalAlignment::Center);

            panel.Child(label);

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
'@

$functionText =
    $functionText.Substring(0, $functionText.Length - 1) +
    $overlayBlock +
    "    }"

$backup = "$view.before-intermediate-taxiway-panels"
if (-not (Test-Path $backup))
{
    Copy-Item $view $backup
}

$text =
    $text.Substring(0, $start) +
    $functionText +
    $text.Substring($closeBrace + 1)

Set-Content $view $text -Encoding utf8

Write-Host ""
Write-Host "Option-B intermediate taxiway alpha panels applied."
Write-Host ""
Write-Host "Controller nodes remain interactive:"
Write-Host "  F H J 18L"
Write-Host ""
Write-Host "Physical graph nodes now render as smaller alpha panels:"
Write-Host "  F1 H1 J1"
Write-Host ""
Write-Host "Expected test total after configure/build: 74."
