param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$airfieldsRoot =
    Join-Path $RepoRoot "xSimAtc.Airfields"

$header =
    Join-Path $airfieldsRoot "airfield_taxiway_waypoint_route.h"

$testsCmake =
    Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

foreach ($path in @($header, $testsCmake))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

# ------------------------------------------------------------
# 1) Discover the repo's actual AirfieldWaypoint definition.
# ------------------------------------------------------------

$definitionMatches =
    Get-ChildItem $airfieldsRoot -Recurse -File |
    Where-Object {
        $_.Extension -in @(".h", ".hpp", ".cpp")
    } |
    Select-String `
        -Pattern 'struct\s+AirfieldWaypoint\b|class\s+AirfieldWaypoint\b'

if (-not $definitionMatches)
{
    throw "Could not locate the AirfieldWaypoint definition."
}

$definitionFile =
    $definitionMatches |
    Select-Object -ExpandProperty Path -Unique |
    Select-Object -First 1

Write-Host "AirfieldWaypoint definition:"
Write-Host "  $definitionFile"

$headerDir =
    Split-Path $header -Parent

$headerUri =
    New-Object System.Uri(
        ($headerDir.TrimEnd('\') + '\'))

$definitionUri =
    New-Object System.Uri(
        $definitionFile)

$relative =
    $headerUri.MakeRelativeUri(
        $definitionUri).ToString()

$relative =
    [System.Uri]::UnescapeDataString(
        $relative).Replace('\', '/')

Write-Host "Resolved include:"
Write-Host "  $relative"

$headerText =
    Get-Content $header -Raw

if ($headerText -match '__AIRFIELD_WAYPOINT_HEADER__')
{
    $headerText =
        $headerText.Replace(
            '__AIRFIELD_WAYPOINT_HEADER__',
            $relative)

    Set-Content `
        $header `
        $headerText `
        -Encoding utf8
}
else
{
    Write-Host "Waypoint include was already resolved."
}

# ------------------------------------------------------------
# 2) Register focused tests.
# ------------------------------------------------------------

$cmakeText =
    Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_taxiway_waypoint_route_tests\.cpp')
{
    Add-Content `
        -Path $testsCmake `
        -Encoding utf8 `
        -Value @'

# Taxiway graph -> movement waypoint route
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_waypoint_route_tests.cpp
)
'@

    Write-Host "Registered airfield_taxiway_waypoint_route_tests.cpp."
}
else
{
    Write-Host "Waypoint-route tests already registered."
}

Write-Host ""
Write-Host "Taxiway graph -> movement waypoint slice applied."
Write-Host ""
Write-Host "Pipeline now covered:"
Write-Host "  selected node ids"
Write-Host "  -> AirfieldGraph expansion"
Write-Host "  -> taxiway positions"
Write-Host "  -> AirfieldWaypoint vector"
Write-Host ""
Write-Host "No WinUI or movement runtime wiring changed yet."
Write-Host "Expected test total after configure/build: 83."
