param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$path = Join-Path `
    $RepoRoot `
    "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $path)) {
    throw "AirfieldView.xaml.cpp not found: $path"
}

$backup = "$path.before-airfield-force-gcpp"

if (-not (Test-Path $backup)) {
    Copy-Item $path $backup
}

$text = Get-Content -Path $path -Raw

# Remove any previous guarded AirfieldView.g.cpp include.
$text = [regex]::Replace(
    $text,
    '(?ms)\s*#if\s+__has_include\("AirfieldView\.g\.cpp"\)\s*#include\s+"AirfieldView\.g\.cpp"\s*#endif\s*',
    "`r`n"
)

# Remove a stray direct include so we can add one canonical copy.
$text = [regex]::Replace(
    $text,
    '(?m)^\s*#include\s+"AirfieldView\.g\.cpp"\s*\r?\n?',
    ''
)

$needle = '#include "AirfieldView.xaml.h"'
$index = $text.IndexOf($needle)

if ($index -lt 0) {
    throw 'Could not find #include "AirfieldView.xaml.h"'
}

$insertAt = $index + $needle.Length

$text = $text.Insert(
    $insertAt,
    "`r`n#include `"AirfieldView.g.cpp`""
)

Set-Content `
    -Path $path `
    -Value $text `
    -Encoding utf8

Write-Host ""
Write-Host "Forced generated XAML implementation include:"
Write-Host '  #include "AirfieldView.g.cpp"'
Write-Host ""
Write-Host "If the generated file is missing, the next build will now say so directly."
Write-Host ""
Write-Host "Backup:"
Write-Host "  $backup"
