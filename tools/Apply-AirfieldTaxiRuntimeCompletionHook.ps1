param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$runtimeHeader =
    Join-Path $RepoRoot "xSimAtc.Airfields\airfield_taxi_runtime.h"

$testsCmake =
    Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

foreach ($path in @($runtimeHeader, $testsCmake))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

$runtimeText =
    Get-Content $runtimeHeader -Raw

$backup =
    "$runtimeHeader.before-completion-hook"

if (-not (Test-Path $backup))
{
    Copy-Item $runtimeHeader $backup
}

# ------------------------------------------------------------
# Add <functional> if needed.
# ------------------------------------------------------------

if ($runtimeText -notmatch '(?m)^#include\s+<functional>\s*$')
{
    $lastInclude =
        [regex]::Matches(
            $runtimeText,
            '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    if (-not $lastInclude)
    {
        throw "Could not find include block in airfield_taxi_runtime.h"
    }

    $insertAt =
        $lastInclude.Index +
        $lastInclude.Length

    $runtimeText =
        $runtimeText.Insert(
            $insertAt,
            "`r`n#include <functional>")
}

# ------------------------------------------------------------
# Add one forwarding API next to movement().
# This keeps WinUI from reaching through runtime.movement().
# ------------------------------------------------------------

if ($runtimeText -notmatch 'set_completed_callback\s*\(')
{
    $anchor = @'
        [[nodiscard]]
        AirfieldMovement& movement() noexcept
        {
            return movement_;
        }
'@

    $replacement = @'
        void set_completed_callback(
            std::function<void()> callback)
        {
            movement_.set_completed_callback(
                std::move(callback));
        }

        [[nodiscard]]
        AirfieldMovement& movement() noexcept
        {
            return movement_;
        }
'@

    if (-not $runtimeText.Contains($anchor))
    {
        throw "Could not find AirfieldTaxiRuntime::movement() anchor."
    }

    $runtimeText =
        $runtimeText.Replace(
            $anchor,
            $replacement)
}

# Ensure <utility> for std::move.
if ($runtimeText -notmatch '(?m)^#include\s+<utility>\s*$')
{
    $lastInclude =
        [regex]::Matches(
            $runtimeText,
            '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    $insertAt =
        $lastInclude.Index +
        $lastInclude.Length

    $runtimeText =
        $runtimeText.Insert(
            $insertAt,
            "`r`n#include <utility>")
}

Set-Content `
    -Path $runtimeHeader `
    -Value $runtimeText `
    -Encoding utf8

# ------------------------------------------------------------
# Register focused tests.
# ------------------------------------------------------------

$cmakeText =
    Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_taxi_runtime_completion_hook_tests\.cpp')
{
    Add-Content `
        -Path $testsCmake `
        -Encoding utf8 `
        -Value @'

# Taxi runtime completion hook
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxi_runtime_completion_hook_tests.cpp
)
'@

    Write-Host "Registered taxi runtime completion-hook tests."
}
else
{
    Write-Host "Completion-hook tests already registered."
}

Write-Host ""
Write-Host "AirfieldTaxiRuntime completion-hook slice applied."
Write-Host ""
Write-Host "New public API:"
Write-Host "  set_completed_callback(std::function<void()>)"
Write-Host ""
Write-Host "This prepares the WinUI composition layer to bind:"
Write-Host "  taxi completion -> real AircraftBase phase transition"
Write-Host ""
Write-Host "Expected test total: 95."
