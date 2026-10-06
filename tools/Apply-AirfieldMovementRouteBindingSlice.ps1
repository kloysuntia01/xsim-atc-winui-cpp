param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$airfieldsRoot =
    Join-Path $RepoRoot "xSimAtc.Airfields"

$header =
    Join-Path $airfieldsRoot "airfield_movement_route_binding.h"

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
# 1) Discover the actual AirfieldMovement definition.
# ------------------------------------------------------------

$movementMatches =
    Get-ChildItem $airfieldsRoot -Recurse -File |
    Where-Object {
        $_.Extension -in @(".h", ".hpp", ".cpp")
    } |
    Select-String `
        -Pattern 'class\s+AirfieldMovement\b|struct\s+AirfieldMovement\b'

if (-not $movementMatches)
{
    throw "Could not locate AirfieldMovement definition."
}

$movementFile =
    $movementMatches |
    Select-Object -ExpandProperty Path -Unique |
    Select-Object -First 1

Write-Host "AirfieldMovement definition:"
Write-Host "  $movementFile"

$headerDir =
    Split-Path $header -Parent

$headerUri =
    New-Object System.Uri(
        ($headerDir.TrimEnd('\') + '\'))

$movementUri =
    New-Object System.Uri(
        $movementFile)

$relative =
    $headerUri.MakeRelativeUri(
        $movementUri).ToString()

$relative =
    [System.Uri]::UnescapeDataString(
        $relative).Replace('\', '/')

Write-Host "Resolved include:"
Write-Host "  $relative"

$headerText =
    Get-Content $header -Raw

if ($headerText -match '__AIRFIELD_MOVEMENT_HEADER__')
{
    $headerText =
        $headerText.Replace(
            '__AIRFIELD_MOVEMENT_HEADER__',
            $relative)

    Set-Content `
        $header `
        $headerText `
        -Encoding utf8
}
else
{
    Write-Host "Movement include already resolved."
}

# ------------------------------------------------------------
# 2) Register tests.
# ------------------------------------------------------------

$cmakeText =
    Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_movement_route_binding_tests\.cpp')
{
    Add-Content `
        -Path $testsCmake `
        -Encoding utf8 `
        -Value @'

# Selected taxiway route -> AirfieldMovement
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_movement_route_binding_tests.cpp
)
'@

    Write-Host "Registered airfield_movement_route_binding_tests.cpp."
}
else
{
    Write-Host "Movement route binding tests already registered."
}

Write-Host ""
Write-Host "Selected route -> AirfieldMovement binding slice applied."
Write-Host ""
Write-Host "Pipeline now covered:"
Write-Host "  selected node ids"
Write-Host "  -> graph-expanded AirfieldWaypoints"
Write-Host "  -> AirfieldMovement.set_route()"
Write-Host "  -> optional start()"
Write-Host ""
Write-Host "No WinUI runtime wiring changed yet."
Write-Host "Expected test total after configure/build: 86."
