param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$routeHeader = Join-Path $RepoRoot "xSimAtc.Airfields\Routing\airfield_route.h"
$viewHeader  = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.h"

foreach ($path in @($routeHeader, $viewHeader))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

# ------------------------------------------------------------
# 1) Exact std::min macro-collision repair.
# ------------------------------------------------------------

$routeBackup = "$routeHeader.before-exact-min-fix"
if (-not (Test-Path $routeBackup))
{
    Copy-Item $routeHeader $routeBackup
}

$routeText = Get-Content $routeHeader -Raw

$oldMin = @'
        const auto segment_index =
            std::min(
                static_cast<std::size_t>(scaled),
                segment_count - 1);
'@

$newMin = @'
        const auto segment_index =
            (std::min)(
                static_cast<std::size_t>(scaled),
                segment_count - 1);
'@

if ($routeText.Contains($newMin))
{
    Write-Host "airfield_route.h already uses macro-safe std::min."
}
elseif ($routeText.Contains($oldMin))
{
    $routeText = $routeText.Replace($oldMin, $newMin)
    Set-Content $routeHeader $routeText -Encoding utf8
    Write-Host "Applied exact macro-safe std::min repair."
}
else
{
    throw "Expected std::min block not found in airfield_route.h"
}

# ------------------------------------------------------------
# 2) Move taxi_ui_runtime_ out of factory_implementation
#    and into implementation::AirfieldView, where on_node_clicked()
#    can access it.
# ------------------------------------------------------------

$viewBackup = "$viewHeader.before-runtime-member-move"
if (-not (Test-Path $viewBackup))
{
    Copy-Item $viewHeader $viewBackup
}

$viewText = Get-Content $viewHeader -Raw

# Remove the bad member from factory_implementation, allowing the current
# compact formatting seen in the user's file.
$viewText = [regex]::Replace(
    $viewText,
    '\s*xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime\s+taxi_ui_runtime_;\s*',
    "`r`n",
    [System.Text.RegularExpressions.RegexOptions]::None
)

# Insert member into the implementation struct directly after view_model_.
$anchor = @'
        winrt::xSimAtc_Terminal_WinUI::AirfieldViewModel
            view_model_{ nullptr };
'@

$replacement = @'
        winrt::xSimAtc_Terminal_WinUI::AirfieldViewModel
            view_model_{ nullptr };

        ::xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime
            taxi_ui_runtime_;
'@

if ($viewText.Contains($replacement))
{
    Write-Host "AirfieldTaxiUiRuntime member already exists in implementation::AirfieldView."
}
elseif ($viewText.Contains($anchor))
{
    $viewText = $viewText.Replace($anchor, $replacement)
}
else
{
    throw "Could not find view_model_ anchor in AirfieldView.xaml.h"
}

Set-Content $viewHeader $viewText -Encoding utf8

Write-Host ""
Write-Host "FDX606 runtime compile fix v2 applied."
Write-Host ""
Write-Host "Fixed:"
Write-Host "  1) std::min -> (std::min) in airfield_route.h"
Write-Host "  2) taxi_ui_runtime_ moved from factory_implementation"
Write-Host "     into implementation::AirfieldView"
Write-Host ""
Write-Host "Expected regression baseline remains 90/90."
