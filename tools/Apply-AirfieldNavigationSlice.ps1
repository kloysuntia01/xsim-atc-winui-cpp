param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$ui = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI"

function Read-Raw([string]$path) {
    if (-not (Test-Path $path)) {
        throw "Required file not found: $path"
    }

    return Get-Content -Path $path -Raw
}

function Write-Raw(
    [string]$path,
    [string]$content) {

    Set-Content `
        -Path $path `
        -Value $content `
        -Encoding utf8
}

function Backup-Once([string]$path) {
    $backup = "$path.before-airfield-navigation"

    if (-not (Test-Path $backup)) {
        Copy-Item $path $backup
    }
}

# ------------------------------------------------------------------
# MainViewModel.idl
# ------------------------------------------------------------------
$path = Join-Path $ui "MainViewModel.idl"
Backup-Once $path
$text = Read-Raw $path

if ($text -notmatch "NavigateToAirfieldCommand") {
    $pattern =
        '(?m)^([^\r\n]*NavigateToTrackingsCommand[^\r\n]*\r?\n)'

    $m = [regex]::Match($text, $pattern)

    if (-not $m.Success) {
        throw "Could not find NavigateToTrackingsCommand in MainViewModel.idl"
    }

    $airfieldLine =
        $m.Groups[1].Value.Replace(
            "NavigateToTrackingsCommand",
            "NavigateToAirfieldCommand")

    $text = $text.Insert(
        $m.Index + $m.Length,
        $airfieldLine)

    Write-Raw $path $text
}

# ------------------------------------------------------------------
# MainViewModel.h
# Add command getter + backing ICommand without changing existing
# Tower/Trackings navigation.
# ------------------------------------------------------------------
$path = Join-Path $ui "MainViewModel.h"
Backup-Once $path
$text = Read-Raw $path

if ($text -notmatch "NavigateToAirfieldCommand") {
    $pattern =
        '(?m)^([^\r\n]*NavigateToTrackingsCommand\s*\([^\r\n]*\)[^\r\n]*;\s*\r?\n)'

    $m = [regex]::Match($text, $pattern)

    if (-not $m.Success) {
        throw "Could not find NavigateToTrackingsCommand declaration in MainViewModel.h"
    }

    $line =
        $m.Groups[1].Value.Replace(
            "NavigateToTrackingsCommand",
            "NavigateToAirfieldCommand")

    $text = $text.Insert(
        $m.Index + $m.Length,
        $line)
}

if ($text -notmatch "navigate_to_airfield_command_") {
    $pattern =
        '(?m)^([^\r\n]*navigate_to_trackings_command_[^\r\n]*;\s*\r?\n)'

    $m = [regex]::Match($text, $pattern)

    if ($m.Success) {
        $line =
            $m.Groups[1].Value.Replace(
                "navigate_to_trackings_command_",
                "navigate_to_airfield_command_")

        $text = $text.Insert(
            $m.Index + $m.Length,
            $line)
    }
    else {
        $pattern =
            '(?m)^([^\r\n]*selected_view_model_[^\r\n]*;\s*\r?\n)'

        $m = [regex]::Match($text, $pattern)

        if (-not $m.Success) {
            throw "Could not locate MainViewModel private members."
        }

        $indent =
            ([regex]::Match(
                $m.Groups[1].Value,
                '^\s*')).Value

        $line =
            $indent +
            'Microsoft::UI::Xaml::Input::ICommand navigate_to_airfield_command_{ nullptr };' +
            "`r`n"

        $text = $text.Insert(
            $m.Index + $m.Length,
            $line)
    }
}

Write-Raw $path $text

# ------------------------------------------------------------------
# App.xaml
# Add template and switch only the resource instance to the new
# selector class. Existing selector source remains untouched.
# ------------------------------------------------------------------
$path = Join-Path $ui "App.xaml"
Backup-Once $path
$text = Read-Raw $path

if ($text -notmatch 'x:Key="AirfieldViewTemplate"') {
    $template = @"

        <DataTemplate x:Key="AirfieldViewTemplate">
            <local:AirfieldView />
        </DataTemplate>
"@

    $selectorIndex =
        $text.IndexOf("<local:ViewTemplateSelector")

    if ($selectorIndex -lt 0) {
        throw "Could not find ViewTemplateSelector resource in App.xaml"
    }

    $lineStart =
        $text.LastIndexOf(
            "`n",
            $selectorIndex)

    $text = $text.Insert(
        $lineStart + 1,
        $template + "`r`n")
}

$text =
    $text.Replace(
        "<local:ViewTemplateSelector",
        "<local:AirfieldAwareViewTemplateSelector")

if ($text -notmatch 'AirfieldTemplate="\{StaticResource AirfieldViewTemplate\}"') {
    $selectorIndex =
        $text.IndexOf(
            "<local:AirfieldAwareViewTemplateSelector")

    if ($selectorIndex -lt 0) {
        throw "Could not find AirfieldAwareViewTemplateSelector in App.xaml"
    }

    $close =
        $text.IndexOf(
            "/>",
            $selectorIndex)

    if ($close -lt 0) {
        throw "Could not find selector closing '/>' in App.xaml"
    }

    $insert =
        "`r`n            AirfieldTemplate=`"{StaticResource AirfieldViewTemplate}`""

    $text =
        $text.Insert(
            $close,
            $insert + "`r`n        ")
}

Write-Raw $path $text

# ------------------------------------------------------------------
# MainWindow.xaml
# Clone the existing Trackings nav button so visual layout/rail
# behavior remains identical.
# ------------------------------------------------------------------
$path = Join-Path $ui "MainWindow.xaml"
Backup-Once $path
$text = Read-Raw $path

if ($text -notmatch "NavigateToAirfieldCommand") {
    $buttonPattern =
        '(?s)<Button\b(?:(?!<Button\b).)*?NavigateToTrackingsCommand(?:(?!<Button\b).)*?</Button>'

    $m =
        [regex]::Match(
            $text,
            $buttonPattern)

    if (-not $m.Success) {
        # Support a self-closing Button if current shell uses one.
        $buttonPattern =
            '(?s)<Button\b(?:(?!<Button\b).)*?NavigateToTrackingsCommand(?:(?!<Button\b).)*?/>'

        $m =
            [regex]::Match(
                $text,
                $buttonPattern)
    }

    if (-not $m.Success) {
        throw "Could not locate Trackings navigation Button in MainWindow.xaml"
    }

    $airfieldButton = $m.Value

    $airfieldButton =
        $airfieldButton.Replace(
            "NavigateToTrackingsCommand",
            "NavigateToAirfieldCommand")

    $airfieldButton =
        $airfieldButton.Replace(
            "Trackings",
            "Airfield")

    $airfieldButton =
        $airfieldButton.Replace(
            "Tracking",
            "Airfield")

    $text =
        $text.Insert(
            $m.Index + $m.Length,
            "`r`n" + $airfieldButton)

    Write-Raw $path $text
}

# ------------------------------------------------------------------
# xSimAtc.Terminal.WinUI.vcxproj
# ------------------------------------------------------------------
$path =
    Join-Path `
        $ui `
        "xSimAtc.Terminal.WinUI.vcxproj"

Backup-Once $path
$text = Read-Raw $path

$marker =
    "<!-- xSimAtc Airfield navigation slice -->"

if ($text -notmatch "AirfieldAwareViewTemplateSelector.idl") {
    $items = @"
  $marker
  <ItemGroup>
    <Midl Include="AirfieldViewModel.idl" />
    <Midl Include="AirfieldAwareViewTemplateSelector.idl" />

    <ClInclude Include="AirfieldNavigationCommand.h" />
    <ClInclude Include="AirfieldAwareViewTemplateSelector.h" />

    <ClCompile Include="MainViewModel.Airfield.cpp" />
    <ClCompile Include="AirfieldAwareViewTemplateSelector.cpp" />
  </ItemGroup>
"@

    $closing =
        $text.LastIndexOf("</Project>")

    if ($closing -lt 0) {
        throw "Could not find </Project> in WinUI project"
    }

    $text =
        $text.Insert(
            $closing,
            $items + "`r`n")

    # AirfieldViewModel h/cpp were already registered by the prior slice.
    # Only add its IDL here.
    Write-Raw $path $text
}

Write-Host ""
Write-Host "Airfield navigation slice installed."
Write-Host ""
Write-Host "Flow:"
Write-Host "  Airfield button"
Write-Host "    -> NavigateToAirfieldCommand"
Write-Host "    -> AirfieldViewModel"
Write-Host "    -> AirfieldAwareViewTemplateSelector"
Write-Host "    -> AirfieldView"
Write-Host ""
Write-Host "Backups use suffix:"
Write-Host "  .before-airfield-navigation"
