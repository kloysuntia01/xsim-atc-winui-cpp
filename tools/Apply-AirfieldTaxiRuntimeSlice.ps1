param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsCmake =
    Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

if (-not (Test-Path $testsCmake))
{
    throw "Missing test CMake file: $testsCmake"
}

$cmakeText =
    Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_taxi_runtime_tests\.cpp')
{
    Add-Content `
        -Path $testsCmake `
        -Encoding utf8 `
        -Value @'

# Live taxi runtime coordinator
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxi_runtime_tests.cpp
)
'@

    Write-Host "Registered airfield_taxi_runtime_tests.cpp."
}
else
{
    Write-Host "Taxi runtime tests already registered."
}

Write-Host ""
Write-Host "AirfieldTaxiRuntime slice applied."
Write-Host ""
Write-Host "Runtime flow now covered:"
Write-Host "  selected route"
Write-Host "  -> graph-expanded waypoints"
Write-Host "  -> AirfieldMovement"
Write-Host "  -> start"
Write-Host "  -> advance"
Write-Host "  -> destination/completion"
Write-Host ""
Write-Host "This remains WinUI-independent."
Write-Host "The next slice can bind one AirfieldTaxiRuntime instance to FDX606 and a UI tick."
Write-Host ""
Write-Host "Expected test total after configure/build: 90."
