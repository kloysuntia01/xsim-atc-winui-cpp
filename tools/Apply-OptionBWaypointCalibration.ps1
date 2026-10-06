param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$layout =
    Join-Path $RepoRoot "xSimAtc.Airfields\airfield_taxiway_layout.h"

$testsCmake =
    Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

foreach ($path in @($layout, $testsCmake))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

$layoutText = Get-Content $layout -Raw

$backup = "$layout.before-optionb-waypoint-calibration-v2"
if (-not (Test-Path $backup))
{
    Copy-Item $layout $backup
}

$replacements = @(
    @{
        Old = 'TaxiwayNode{ "F1",  { 0.595, 0.520 } },'
        New = 'TaxiwayNode{ "F1",  { 0.595, 0.329 } },'
    },
    @{
        Old = 'TaxiwayNode{ "H1",  { 0.470, 0.690 } },'
        New = 'TaxiwayNode{ "H1",  { 0.470, 0.729 } },'
    },
    @{
        Old = 'TaxiwayNode{ "J1",  { 0.360, 0.835 } },'
        New = 'TaxiwayNode{ "J1",  { 0.400, 0.835 } },'
    }
)

foreach ($r in $replacements)
{
    if ($layoutText.Contains($r.New))
    {
        Write-Host "Already calibrated: $($r.New)"
        continue
    }

    if (-not $layoutText.Contains($r.Old))
    {
        throw "Expected exact node initializer not found: $($r.Old)"
    }

    $layoutText = $layoutText.Replace($r.Old, $r.New)
    Write-Host "Calibrated: $($r.New)"
}

Set-Content `
    -Path $layout `
    -Value $layoutText `
    -Encoding utf8

$cmakeText = Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_optionb_waypoint_calibration_tests\.cpp')
{
    Add-Content `
        -Path $testsCmake `
        -Encoding utf8 `
        -Value @'

# Option-B physical waypoint calibration
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_optionb_waypoint_calibration_tests.cpp
)
'@

    Write-Host "Registered Option-B waypoint calibration tests."
}
else
{
    Write-Host "Calibration tests already registered."
}

Write-Host ""
Write-Host "Option-B waypoint calibration v2 applied."
Write-Host ""
Write-Host "  F1 -> { 0.595, 0.329 }"
Write-Host "  H1 -> { 0.470, 0.729 }"
Write-Host "  J1 -> { 0.400, 0.835 }"
Write-Host ""
Write-Host "Expected test total: 93."
