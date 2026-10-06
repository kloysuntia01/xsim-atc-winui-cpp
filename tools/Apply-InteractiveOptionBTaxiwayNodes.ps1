param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsCmake = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"
$view = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

foreach ($path in @($testsCmake, $view))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

# ------------------------------------------------------------
# 1) Register selection tests.
# ------------------------------------------------------------

$cmakeText = Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_taxiway_selection_tests\.cpp')
{
    Add-Content -Path $testsCmake -Encoding utf8 -Value @'

# Interactive Option-B taxiway nodes
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_selection_tests.cpp
)
'@

    Write-Host "Registered airfield_taxiway_selection_tests.cpp."
}
else
{
    Write-Host "Taxiway selection tests already registered."
}

# ------------------------------------------------------------
# 2) Make every physical/intersection panel clickable.
#    Existing F/H/J/18L buttons remain untouched.
# ------------------------------------------------------------

$text = Get-Content $view -Raw

$selectionInclude =
    '#include "../xSimAtc.Airfields/airfield_taxiway_selection.h"'

if ($text -notmatch [regex]::Escape($selectionInclude))
{
    $lastInclude =
        [regex]::Matches(
            $text,
            '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    if (-not $lastInclude)
    {
        throw "Could not locate include block."
    }

    $insertAt =
        $lastInclude.Index +
        $lastInclude.Length

    $text =
        $text.Insert(
            $insertAt,
            "`r`n$selectionInclude")
}

$signature = "void AirfieldView::render_nodes()"
$start = $text.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find render_nodes()."
}

$openBrace = $text.IndexOf("{", $start)
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

$functionText =
    $text.Substring(
        $start,
        $closeBrace - $start + 1)

if ($functionText -match 'OPTION_B_INTERACTIVE_TAXIWAY_PANELS')
{
    Write-Host "Interactive taxiway panels already applied."
    Write-Host "Expected test total remains 80."
    exit 0
}

$marker = '// OPTION_B_INTERMEDIATE_TAXIWAY_PANELS'
$markerIndex = $functionText.IndexOf($marker)

if ($markerIndex -lt 0)
{
    throw "Could not find the existing intermediate taxiway panel block."
}

$loopStart =
    $functionText.LastIndexOf(
        "        for (",
        $markerIndex)

if ($loopStart -lt 0)
{
    # Marker comes before loop in the current implementation.
    $loopStart =
        $functionText.IndexOf(
            "        for (",
            $markerIndex)
}

if ($loopStart -lt 0)
{
    throw "Could not find intermediate-node loop."
}

# Find the loop's opening brace and brace-match only that loop.
$loopOpen = $functionText.IndexOf("{", $loopStart)

if ($loopOpen -lt 0)
{
    throw "Could not find intermediate-node loop opening brace."
}

$loopDepth = 0
$loopClose = -1

for ($i = $loopOpen; $i -lt $functionText.Length; $i++)
{
    if ($functionText[$i] -eq '{')
    {
        $loopDepth++
    }
    elseif ($functionText[$i] -eq '}')
    {
        $loopDepth--

        if ($loopDepth -eq 0)
        {
            $loopClose = $i
            break
        }
    }
}

if ($loopClose -lt 0)
{
    throw "Could not find intermediate-node loop closing brace."
}

# Include the marker comment in the replacement if it precedes the loop.
$replaceStart = [Math]::Min($markerIndex, $loopStart)
$replaceLength = $loopClose - $replaceStart + 1

$newBlock = @'
        // OPTION_B_INTERACTIVE_TAXIWAY_PANELS
        //
        // Every physical taxiway/intersection node is now a route-selection
        // target. The original controller nodes F/H/J/18L remain the larger
        // buttons above; these smaller buttons expose the rest of the graph.
        for (const auto* taxiway_node :
             xsim::airfields::taxiway_overlay::intermediate_nodes())
        {
            Microsoft::UI::Xaml::Controls::Button panel;

            panel.Width(30.0);
            panel.Height(22.0);
            panel.Padding(thickness(0.0));
            panel.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{ 4.0 });

            const bool selected =
                vm->is_route_node_selected(
                    std::string{ taxiway_node->id });

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
                std::string{ taxiway_node->id };

            panel.Click(
                [this, node_id](
                    auto&&,
                    auto&&)
                {
                    on_node_clicked(node_id);
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
'@

$functionText =
    $functionText.Substring(0, $replaceStart) +
    $newBlock +
    $functionText.Substring(
        $replaceStart + $replaceLength)

$backup =
    "$view.before-interactive-taxiway-panels"

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
Write-Host "Interactive Option-B taxiway nodes applied."
Write-Host ""
Write-Host "Existing controller buttons:"
Write-Host "  F H J 18L"
Write-Host ""
Write-Host "New clickable graph/intersection buttons:"
Write-Host "  F1 H1 J1 B C D E G K L M N P Q R S"
Write-Host ""
Write-Host "All selections flow through the existing on_node_clicked() route logic."
Write-Host "The live route renderer continues to expand each selected segment through AirfieldGraph."
Write-Host ""
Write-Host "Expected test total after configure/build: 80."
