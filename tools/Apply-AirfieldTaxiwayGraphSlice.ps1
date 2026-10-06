param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsCmake = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

if (-not (Test-Path $testsCmake))
{
    throw "Missing test CMake file: $testsCmake"
}

$cmakeText = Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_graph_tests\.cpp')
{
    $append = @'

# Airfield taxiway graph slice
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_graph_tests.cpp
)
'@

    Add-Content -Path $testsCmake -Value $append -Encoding utf8
    Write-Host "Registered airfield_graph_tests.cpp with xSimAtc.Airfields.Tests."
}
else
{
    Write-Host "airfield_graph_tests.cpp is already registered."
}

Write-Host ""
Write-Host "Added:"
Write-Host "  xSimAtc.Airfields\airfield_edge.h"
Write-Host "  xSimAtc.Airfields\airfield_graph.h"
Write-Host "  xSimAtc.Airfields.Tests\airfield_graph_tests.cpp"
Write-Host ""
Write-Host "This slice is domain-only:"
Write-Host "  AirfieldNode ids become graph vertices"
Write-Host "  AirfieldEdge becomes taxiway connectivity"
Write-Host "  AirfieldGraph.route() expands a logical route through the taxiway network"
Write-Host ""
Write-Host "Expected test total after successful configure/build: 68."
