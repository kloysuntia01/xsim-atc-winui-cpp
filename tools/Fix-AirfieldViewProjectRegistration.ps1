param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$projectPath = Join-Path `
    $RepoRoot `
    "xSimAtc.Terminal.WinUI\xSimAtc.Terminal.WinUI.vcxproj"

if (-not (Test-Path $projectPath)) {
    throw "WinUI project not found: $projectPath"
}

$backup = "$projectPath.before-airfield-view-registration-fix"

if (-not (Test-Path $backup)) {
    Copy-Item $projectPath $backup
}

$text = Get-Content -Path $projectPath -Raw

function Has-Include(
    [string]$content,
    [string]$fileName) {

    return $content -match [regex]::Escape("Include=`"$fileName`"")
}

$entries = @()

if (-not (Has-Include $text "AirfieldView.idl")) {
    $entries += '    <Midl Include="AirfieldView.idl" />'
}

if (-not (Has-Include $text "AirfieldView.xaml")) {
    $entries += '    <Page Include="AirfieldView.xaml" />'
}

if (-not (Has-Include $text "AirfieldView.xaml.h")) {
    $entries += '    <ClInclude Include="AirfieldView.xaml.h" />'
}

if (-not (Has-Include $text "AirfieldView.xaml.cpp")) {
    $entries += '    <ClCompile Include="AirfieldView.xaml.cpp" />'
}

if (-not (Has-Include $text "AirfieldViewModel.h")) {
    $entries += '    <ClInclude Include="AirfieldViewModel.h" />'
}

if (-not (Has-Include $text "AirfieldViewModel.cpp")) {
    $entries += '    <ClCompile Include="AirfieldViewModel.cpp" />'
}

if ($entries.Count -eq 0) {
    Write-Host "AirfieldView project registrations already exist."
    exit 0
}

$block = @"
  <!-- xSimAtc AirfieldView registration fix -->
  <ItemGroup>
$($entries -join "`r`n")
  </ItemGroup>
"@

$closing = $text.LastIndexOf("</Project>")

if ($closing -lt 0) {
    throw "Could not find </Project> in $projectPath"
}

$text = $text.Insert(
    $closing,
    $block + "`r`n")

Set-Content `
    -Path $projectPath `
    -Value $text `
    -Encoding utf8

Write-Host ""
Write-Host "Registered missing AirfieldView WinUI files:"
$entries | ForEach-Object {
    Write-Host "  $_"
}
Write-Host ""
Write-Host "Backup:"
Write-Host "  $backup"
