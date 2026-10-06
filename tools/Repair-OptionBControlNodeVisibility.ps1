param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$viewCpp =
    Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $viewCpp))
{
    throw "Required file not found: $viewCpp"
}

$cppText = Get-Content $viewCpp -Raw

$backup =
    "$viewCpp.before-control-node-visibility-fix"

if (-not (Test-Path $backup))
{
    Copy-Item $viewCpp $backup
}

# ------------------------------------------------------------
# Locate render_nodes() exactly and modify only that function.
# ------------------------------------------------------------

$signature = "void AirfieldView::render_nodes()"
$start = $cppText.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::render_nodes()."
}

$openBrace = $cppText.IndexOf("{", $start)

if ($openBrace -lt 0)
{
    throw "Could not find render_nodes() opening brace."
}

$depth = 0
$closeBrace = -1

for ($i = $openBrace; $i -lt $cppText.Length; $i++)
{
    if ($cppText[$i] -eq '{')
    {
        $depth++
    }
    elseif ($cppText[$i] -eq '}')
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
    $cppText.Substring(
        $start,
        $closeBrace - $start + 1)

if ($functionText -match 'OPTION_B_CONTROL_NODE_VISIBILITY')
{
    Write-Host "Control-node visibility fix already applied."
    exit 0
}

# ------------------------------------------------------------
# Find the larger controller-node button block.
#
# Known current shape uses:
#   button.Width(36.0);
#   button.Height(28.0);
#
# The smaller physical/intermediate nodes use a different size.
# Replace only the first matching 36x28 pair inside render_nodes().
# ------------------------------------------------------------

$sizePattern =
    'button\.Width\(36\.0\);\s*' +
    'button\.Height\(28\.0\);'

$sizeMatch =
    [regex]::Match(
        $functionText,
        $sizePattern)

if (-not $sizeMatch.Success)
{
    Write-Host ""
    Write-Host "Could not find the 36x28 controller-node button block."
    Write-Host "Current render_nodes():"
    Write-Host "------------------------------------------------------------"
    Write-Host $functionText
    Write-Host "------------------------------------------------------------"
    throw "Safe patch aborted."
}

$newSize = @'
button.Width(52.0);
        button.Height(34.0);

        // OPTION_B_CONTROL_NODE_VISIBILITY
        // Controller decision nodes must win the visual/hit-test stack
        // over smaller physical/intermediate waypoint labels.
        Microsoft::UI::Xaml::Controls::Canvas::SetZIndex(
            button,
            100);
'@

$functionText =
    $functionText.Substring(0, $sizeMatch.Index) +
    $newSize +
    $functionText.Substring(
        $sizeMatch.Index + $sizeMatch.Length)

# ------------------------------------------------------------
# Strengthen controller-node border thickness.
# Current control buttons already have selected styling; only
# increase their normal prominence.
# ------------------------------------------------------------

$borderPattern =
    'button\.BorderThickness\(\s*' +
    'thickness\(\s*' +
    'selected\s*\?\s*3\.0\s*:\s*1\.0\s*\)\s*\);'

$borderMatch =
    [regex]::Match(
        $functionText,
        $borderPattern)

if ($borderMatch.Success)
{
    $borderReplacement = @'
button.BorderThickness(
            thickness(
                selected
                    ? 3.0
                    : 2.0));
'@

    $functionText =
        $functionText.Substring(0, $borderMatch.Index) +
        $borderReplacement +
        $functionText.Substring(
            $borderMatch.Index + $borderMatch.Length)
}

# ------------------------------------------------------------
# Replace the controller-node position block with a displaced
# LABEL/HIT TARGET while keeping the domain coordinate untouched.
#
# This avoids F hiding under F1/G and similarly separates
# H/H1 and J/J1. The route geometry does NOT move.
# ------------------------------------------------------------

$positionPattern =
    'Canvas::SetLeft\(\s*button,\s*screen_x\(node\.position\.x\)\s*-\s*17\.0\s*\);\s*' +
    'Canvas::SetTop\(\s*button,\s*screen_y\(node\.position\.y\)\s*-\s*17\.0\s*\);'

$positionMatch =
    [regex]::Match(
        $functionText,
        $positionPattern)

if (-not $positionMatch.Success)
{
    # Try fully-qualified Canvas spelling.
    $positionPattern =
        'Microsoft::UI::Xaml::Controls::Canvas::\s*SetLeft\(\s*button,\s*screen_x\(node\.position\.x\)\s*-\s*17\.0\s*\);\s*' +
        'Microsoft::UI::Xaml::Controls::Canvas::\s*SetTop\(\s*button,\s*screen_y\(node\.position\.y\)\s*-\s*17\.0\s*\);'

    $positionMatch =
        [regex]::Match(
            $functionText,
            $positionPattern)
}

if (-not $positionMatch.Success)
{
    Write-Host ""
    Write-Host "Could not find controller-node Canvas position block."
    Write-Host "The size/ZIndex changes were NOT written."
    throw "Safe patch aborted."
}

$positionReplacement = @'
double label_offset_x = -26.0;
        double label_offset_y = -17.0;

        // Separate controller nodes from nearby physical waypoint labels.
        // Only the visual hit target moves; node.position remains authoritative
        // for routing and aircraft movement.
        if (node.id == "F")
        {
            label_offset_y = 10.0;
        }
        else if (node.id == "H")
        {
            label_offset_y = -42.0;
        }
        else if (node.id == "J")
        {
            label_offset_x = -58.0;
            label_offset_y = -4.0;
        }
        else if (node.id == "18L")
        {
            label_offset_x = -58.0;
            label_offset_y = -4.0;
        }

        Microsoft::UI::Xaml::Controls::Canvas::SetLeft(
            button,
            screen_x(node.position.x) +
                label_offset_x);

        Microsoft::UI::Xaml::Controls::Canvas::SetTop(
            button,
            screen_y(node.position.y) +
                label_offset_y);
'@

$functionText =
    $functionText.Substring(0, $positionMatch.Index) +
    $positionReplacement +
    $functionText.Substring(
        $positionMatch.Index + $positionMatch.Length)

# ------------------------------------------------------------
# Rebuild file only after all required anchors succeeded.
# ------------------------------------------------------------

$cppText =
    $cppText.Substring(0, $start) +
    $functionText +
    $cppText.Substring($closeBrace + 1)

Set-Content `
    -Path $viewCpp `
    -Value $cppText `
    -Encoding utf8

Write-Host ""
Write-Host "Option-B controller-node visibility fix applied."
Write-Host ""
Write-Host "Controller nodes:"
Write-Host "  F / H / J / 18L"
Write-Host "  -> 52x34 hit target"
Write-Host "  -> ZIndex 100"
Write-Host "  -> stronger normal border"
Write-Host "  -> label/hit-target offsets away from F1/H1/J1"
Write-Host ""
Write-Host "Route coordinates are unchanged."
Write-Host "Expected regression baseline remains 95/95."
