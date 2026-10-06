param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$overlayRoot = Split-Path -Parent $scriptRoot

$copies = @(
    @{
        Source = "xSimAtc.Airfields\Routing\airfield_route_selection.h"
        Destination = "xSimAtc.Airfields\Routing\airfield_route_selection.h"
    },
    @{
        Source = "xSimAtc.Airfields.Tests\airfield_route_selection_tests.cpp"
        Destination = "xSimAtc.Airfields.Tests\airfield_route_selection_tests.cpp"
    },
    @{
        Source = "xSimAtc.Terminal.WinUI\AirfieldViewModel.h"
        Destination = "xSimAtc.Terminal.WinUI\AirfieldViewModel.h"
    },
    @{
        Source = "xSimAtc.Terminal.WinUI\AirfieldViewModel.cpp"
        Destination = "xSimAtc.Terminal.WinUI\AirfieldViewModel.cpp"
    },
    @{
        Source = "xSimAtc.Terminal.WinUI\AirfieldView.xaml.h"
        Destination = "xSimAtc.Terminal.WinUI\AirfieldView.xaml.h"
    },
    @{
        Source = "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"
        Destination = "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"
    }
)

foreach ($copy in $copies)
{
    $source = Join-Path $overlayRoot $copy.Source
    $destination = Join-Path $RepoRoot $copy.Destination
    $destinationDirectory = Split-Path -Parent $destination

    New-Item `
        -ItemType Directory `
        -Path $destinationDirectory `
        -Force | Out-Null

    if (Test-Path $destination)
    {
        $backup = "$destination.before-airfield-interactive-route"

        if (-not (Test-Path $backup))
        {
            Copy-Item `
                -Path $destination `
                -Destination $backup
        }
    }

    Copy-Item `
        -Path $source `
        -Destination $destination `
        -Force
}

$testsCmake = Join-Path `
    $RepoRoot `
    "xSimAtc.Airfields.Tests\CMakeLists.txt"

if (-not (Test-Path $testsCmake))
{
    throw "Airfields test CMakeLists.txt not found: $testsCmake"
}

$cmake = Get-Content `
    -Path $testsCmake `
    -Raw

if ($cmake -notmatch 'airfield_route_selection_tests\.cpp')
{
    if ($cmake -match 'airfield_route_tests\.cpp')
    {
        $cmake = $cmake -replace `
            'airfield_route_tests\.cpp', `
            "airfield_route_tests.cpp`r`n    airfield_route_selection_tests.cpp"
    }
    else
    {
        throw "Could not locate airfield_route_tests.cpp in Airfields.Tests CMakeLists.txt"
    }

    Set-Content `
        -Path $testsCmake `
        -Value $cmake `
        -Encoding utf8
}

Write-Host ""
Write-Host "Airfield interactive-route slice applied."
Write-Host ""
Write-Host "Behavior:"
Write-Host "  1. Click FDX606 to select the aircraft."
Write-Host "  2. Click F, H, J, 18L."
Write-Host "  3. Selected nodes turn orange/white."
Write-Host "  4. Route summary builds in click order."
Write-Host "  5. Yellow route segments connect selected nodes."
Write-Host ""
Write-Host "No animation is included in this slice."
Write-Host ""
Write-Host "Expected test count after successful build: 58."
