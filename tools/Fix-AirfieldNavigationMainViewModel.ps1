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

$backup = "$path.before-airfield-nav-header-fix"

if (-not (Test-Path $backup)) {
    Copy-Item $path $backup
}

$text = Get-Content -Path $path -Raw

# Fix the malformed Airfield command getter declaration.
$text = [regex]::Replace(
    $text,
    '(?m)^[ \t]*(?:[^\r\n]*\s+)?NavigateToAirfieldCommand\s*\(\s*\)\s*;\s*$',
    '        Microsoft::UI::Xaml::Input::ICommand NavigateToAirfieldCommand();'
)

# Remove any malformed/duplicate Airfield backing-field declaration.
$text = [regex]::Replace(
    $text,
    '(?m)^[ \t]*[^\r\n;]*navigate_to_airfield_command_[^\r\n;]*;\s*\r?\n?',
    ''
)

# Insert one canonical backing field immediately after the existing
# Trackings command member when available.
$trackingsField = [regex]::Match(
    $text,
    '(?m)^(?<indent>[ \t]*)(?<line>[^\r\n;]*navigate_to_trackings_command_[^\r\n;]*;)\s*$'
)

if ($trackingsField.Success) {
    $indent = $trackingsField.Groups['indent'].Value

    $field =
        "`r`n" +
        $indent +
        'Microsoft::UI::Xaml::Input::ICommand navigate_to_airfield_command_{ nullptr };'

    $text = $text.Insert(
        $trackingsField.Index + $trackingsField.Length,
        $field)
}
else {
    # Fallback: place it before selected_view_model_.
    $selected = [regex]::Match(
        $text,
        '(?m)^(?<indent>[ \t]*)(?<line>[^\r\n;]*selected_view_model_[^\r\n;]*;)\s*$'
    )

    if (-not $selected.Success) {
        throw "Could not locate navigation backing fields in MainViewModel.h"
    }

    $indent = $selected.Groups['indent'].Value

    $field =
        $indent +
        'Microsoft::UI::Xaml::Input::ICommand navigate_to_airfield_command_{ nullptr };' +
        "`r`n"

    $text = $text.Insert(
        $selected.Index,
        $field)
}

Set-Content `
    -Path $path `
    -Value $text `
    -Encoding utf8

Write-Host ""
Write-Host "Repaired MainViewModel.h:"
Write-Host "  ICommand NavigateToAirfieldCommand();"
Write-Host "  ICommand navigate_to_airfield_command_{ nullptr };"
Write-Host ""
Write-Host "Backup:"
Write-Host "  $backup"
