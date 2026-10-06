param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$path = Join-Path `
    $RepoRoot `
    "xSimAtc.Terminal.WinUI\MainViewModel.h"

if (-not (Test-Path $path)) {
    throw "MainViewModel.h not found: $path"
}

$backup = "$path.before-airfield-getter-fix"

if (-not (Test-Path $backup)) {
    Copy-Item $path $backup
}

$lines = Get-Content -Path $path

# Remove every malformed/old Airfield getter declaration line.
$lines = @(
    $lines |
    Where-Object {
        $_ -notmatch '\bNavigateToAirfieldCommand\s*\('
    }
)

# Find the existing, known-good Trackings getter.
$trackingsIndex = -1

for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match '\bNavigateToTrackingsCommand\s*\(') {
        $trackingsIndex = $i
        break
    }
}

if ($trackingsIndex -lt 0) {
    throw "Could not find NavigateToTrackingsCommand() in MainViewModel.h"
}

$indent =
    ([regex]::Match(
        $lines[$trackingsIndex],
        '^\s*')).Value

$insert = @(
    "$indent" + "Microsoft::UI::Xaml::Input::ICommand",
    "$indent" + "NavigateToAirfieldCommand();"
)

$before = @()

if ($trackingsIndex -ge 0) {
    $before = $lines[0..$trackingsIndex]
}

$after = @()

if ($trackingsIndex + 1 -lt $lines.Count) {
    $after = $lines[($trackingsIndex + 1)..($lines.Count - 1)]
}

$newLines = @(
    $before
    $insert
    $after
)

Set-Content `
    -Path $path `
    -Value $newLines `
    -Encoding utf8

Write-Host ""
Write-Host "Airfield getter repaired:"
Write-Host ""
Write-Host "  Microsoft::UI::Xaml::Input::ICommand"
Write-Host "  NavigateToAirfieldCommand();"
Write-Host ""
Write-Host "Inserted immediately after NavigateToTrackingsCommand()."
Write-Host ""
Write-Host "Backup:"
Write-Host "  $backup"
