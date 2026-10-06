param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp",
    [string]$PublishedRoot = "C:\Dev\Xsim.Dev"
)

$ErrorActionPreference = "Stop"

$source = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\Assets\Airfield\airfield-layout.svg"
$targetDir = Join-Path $PublishedRoot "Assets\Airfield"
$target = Join-Path $targetDir "airfield-layout.svg"

if (-not (Test-Path $source))
{
    throw "Missing SVG: $source"
}

New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
Copy-Item $source $target -Force

if (-not (Test-Path $target))
{
    throw "Failed to copy SVG to runtime location: $target"
}

Write-Host ""
Write-Host "Inline-style SVG installed."
Write-Host "Source : $source"
Write-Host "Runtime: $target"
Write-Host ""
Write-Host "This removes CSS classes/style blocks and uses explicit fill/stroke attributes."
Write-Host "Expected test baseline remains 60/60."
