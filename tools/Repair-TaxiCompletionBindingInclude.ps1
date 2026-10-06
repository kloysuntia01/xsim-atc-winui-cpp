param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsRoot = Join-Path $RepoRoot "xSimAtc.Airfields.Tests"
$airfieldsRoot = Join-Path $RepoRoot "xSimAtc.Airfields"

$test = Get-ChildItem $testsRoot -Recurse -Filter "airfield_waypoint_movement_tests.cpp" |
    Select-Object -First 1

$header = Get-ChildItem $airfieldsRoot -Recurse -Filter "airfield_movement_completion_binding.h" |
    Select-Object -First 1

if (-not $test)
{
    throw "Could not find airfield_waypoint_movement_tests.cpp"
}

if (-not $header)
{
    throw "Could not find airfield_movement_completion_binding.h"
}

Write-Host "Test   : $($test.FullName)"
Write-Host "Header : $($header.FullName)"

# Compute a relative path using System.Uri, which works on older Windows PowerShell/.NET Framework.
$testDir = $test.Directory.FullName

if (-not $testDir.EndsWith([System.IO.Path]::DirectorySeparatorChar))
{
    $testDir += [System.IO.Path]::DirectorySeparatorChar
}

$baseUri   = New-Object System.Uri($testDir)
$headerUri = New-Object System.Uri($header.FullName)

$relative = $baseUri.MakeRelativeUri($headerUri).ToString()
$relative = [System.Uri]::UnescapeDataString($relative)
$relative = $relative.Replace("\", "/")

$text = Get-Content $test.FullName -Raw

$oldPattern = '(?m)^\s*#include\s+"airfield_movement_completion_binding\.h"\s*$'
$newInclude = '#include "' + $relative + '"'

if ($text -match $oldPattern)
{
    $text = [regex]::Replace($text, $oldPattern, $newInclude, 1)
}
elseif ($text -match [regex]::Escape($newInclude))
{
    Write-Host "Relative include already present."
}
else
{
    throw "Could not find the old binding include line to replace."
}

Set-Content $test.FullName $text -Encoding utf8

Write-Host ""
Write-Host "Updated include:"
Write-Host "  $newInclude"
Write-Host ""
Write-Host "No production code changed."
Write-Host "Expected test target after rebuild: 64/64."
