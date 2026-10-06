param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$pchPath = Join-Path `
    $RepoRoot `
    "xSimAtc.Terminal.WinUI\pch.h"

if (-not (Test-Path $pchPath)) {
    throw "pch.h not found: $pchPath"
}

$backup = "$pchPath.before-airfield-pch-visibility-fix"

if (-not (Test-Path $backup)) {
    Copy-Item $pchPath $backup
}

$text = Get-Content -Path $pchPath -Raw

$headers = @(
    '#include "AirfieldView.xaml.h"',
    '#include "AirfieldViewModel.h"',
    '#include "AirfieldAwareViewTemplateSelector.h"'
)

$missing = @(
    $headers |
    Where-Object {
        $text -notmatch [regex]::Escape($_)
    }
)

if ($missing.Count -eq 0) {
    Write-Host "Airfield projection headers are already present in pch.h."
    exit 0
}

# Append near the end. This mirrors the existing pattern where
# ViewTemplateSelector.h is made visible to generated XAML metadata code.
$text =
    $text.TrimEnd() +
    "`r`n`r`n// Airfield XAML projection types`r`n" +
    ($missing -join "`r`n") +
    "`r`n"

Set-Content `
    -Path $pchPath `
    -Value $text `
    -Encoding utf8

Write-Host ""
Write-Host "Added to pch.h:"
$missing | ForEach-Object {
    Write-Host "  $_"
}

Write-Host ""
Write-Host "Backup:"
Write-Host "  $backup"
