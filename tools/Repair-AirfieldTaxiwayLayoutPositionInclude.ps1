param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$airfieldsRoot = Join-Path $RepoRoot "xSimAtc.Airfields"
$layoutHeader = Join-Path $airfieldsRoot "airfield_taxiway_layout.h"

if (-not (Test-Path $layoutHeader))
{
    throw "Missing header: $layoutHeader"
}

$matches = Get-ChildItem $airfieldsRoot -Recurse -File |
    Where-Object { $_.Extension -in @(".h", ".hpp", ".cpp") } |
    Select-String -Pattern 'struct\s+AirfieldPosition\b|class\s+AirfieldPosition\b'

if (-not $matches)
{
    throw "Could not find the source file that defines AirfieldPosition."
}

$definitionFile = $matches |
    Select-Object -ExpandProperty Path -Unique |
    Select-Object -First 1

Write-Host "AirfieldPosition definition:"
Write-Host "  $definitionFile"

$layoutDir = Split-Path $layoutHeader -Parent

$layoutUri = New-Object System.Uri(($layoutDir.TrimEnd('\') + '\'))
$definitionUri = New-Object System.Uri($definitionFile)

$relative = $layoutUri.MakeRelativeUri($definitionUri).ToString()
$relative = [System.Uri]::UnescapeDataString($relative)
$relative = $relative.Replace('\', '/')

Write-Host "Relative include:"
Write-Host "  $relative"

$text = Get-Content $layoutHeader -Raw

if ($text -notmatch '#include\s+"airfield_position\.h"')
{
    Write-Host "airfield_position.h include is already absent; no replacement needed."
}
else
{
    $text = $text -replace '#include\s+"airfield_position\.h"', ('#include "' + $relative + '"')
    Set-Content $layoutHeader $text -Encoding utf8

    Write-Host "Replaced invalid airfield_position.h include."
}

Write-Host ""
Write-Host "Taxiway layout include repair applied."
Write-Host "No domain behavior changed."
Write-Host "Expected test target remains 72 after successful rebuild."
