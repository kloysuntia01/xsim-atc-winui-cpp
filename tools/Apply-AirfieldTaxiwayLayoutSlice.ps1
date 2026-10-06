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

if ($cmakeText -notmatch 'airfield_taxiway_layout_tests\.cpp')
{
    $append = @'

# Option-B taxiway layout mapping slice
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_layout_tests.cpp
)
'@

    Add-Content -Path $testsCmake -Value $append -Encoding utf8
    Write-Host "Registered airfield_taxiway_layout_tests.cpp."
}
else
{
    Write-Host "airfield_taxiway_layout_tests.cpp is already registered."
}

Write-Host ""
Write-Host "Added:"
Write-Host "  xSimAtc.Airfields\airfield_taxiway_layout.h"
Write-Host "  xSimAtc.Airfields.Tests\airfield_taxiway_layout_tests.cpp"
Write-Host ""
Write-Host "This slice maps the current Option-B route:"
Write-Host "  F -> H -> J -> 18L"
Write-Host "onto the physical taxiway graph:"
Write-Host "  F -> F1 -> H -> H1 -> J -> J1 -> 18L"
Write-Host ""
Write-Host "No WinUI source is modified yet."
Write-Host "Expected test total: 72."
