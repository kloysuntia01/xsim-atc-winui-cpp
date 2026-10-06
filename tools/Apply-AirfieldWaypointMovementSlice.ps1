param([string]$RepoRoot = "C:\Dev\xsimatc-cpp")
$ErrorActionPreference = "Stop"

$testsCmake = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"
if (-not (Test-Path $testsCmake)) { throw "Missing: $testsCmake" }

$cmake = Get-Content $testsCmake -Raw
$testFile = "airfield_waypoint_movement_tests.cpp"
if ($cmake -notmatch [regex]::Escape($testFile))
{
    if ($cmake -match 'airfield_route_tests\.cpp')
    {
        $cmake = $cmake -replace 'airfield_route_tests\.cpp', "airfield_route_tests.cpp`r`n    $testFile"
        Set-Content $testsCmake $cmake -Encoding utf8
    }
    else { throw "Could not find airfield_route_tests.cpp in $testsCmake" }
}

Write-Host ""
Write-Host "Waypoint + movement domain slice applied."
Write-Host "Route shape: F -> F-H-CENTER -> H -> H-J-CENTER -> J -> J-18L-CENTER -> 18L"
Write-Host "This slice is domain/TDD only; no UI timer yet."
