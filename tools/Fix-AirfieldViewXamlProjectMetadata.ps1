param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$ui = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI"
$proj = Join-Path $ui "xSimAtc.Terminal.WinUI.vcxproj"
$pch  = Join-Path $ui "pch.h"
$cpp  = Join-Path $ui "AirfieldView.xaml.cpp"

foreach ($path in @($proj, $pch, $cpp)) {
    if (-not (Test-Path $path)) {
        throw "Required file not found: $path"
    }
}

# Backups
foreach ($path in @($proj, $pch, $cpp)) {
    $backup = "$path.before-airfield-xaml-metadata-fix"
    if (-not (Test-Path $backup)) {
        Copy-Item $path $backup
    }
}

# ------------------------------------------------------------------
# 1. Normalize AirfieldView project metadata to match TowerView.
# ------------------------------------------------------------------
$xml = Get-Content $proj -Raw

$xml = [regex]::Replace(
    $xml,
    '(?ms)<ClInclude\s+Include="AirfieldView\.xaml\.h"\s*(?:/>|>.*?</ClInclude>)',
    '<ClInclude Include="AirfieldView.xaml.h"><DependentUpon>AirfieldView.xaml</DependentUpon></ClInclude>'
)

$xml = [regex]::Replace(
    $xml,
    '(?ms)<ClCompile\s+Include="AirfieldView\.xaml\.cpp"\s*(?:/>|>.*?</ClCompile>)',
    '<ClCompile Include="AirfieldView.xaml.cpp"><DependentUpon>AirfieldView.xaml</DependentUpon></ClCompile>'
)

$xml = [regex]::Replace(
    $xml,
    '(?ms)<Midl\s+Include="AirfieldView\.idl"\s*(?:/>|>.*?</Midl>)',
    '<Midl Include="AirfieldView.idl"><SubType>Code</SubType><DependentUpon>AirfieldView.xaml</DependentUpon></Midl>'
)

Set-Content $proj $xml -Encoding utf8

# ------------------------------------------------------------------
# 2. Remove temporary Airfield XAML-class visibility includes from PCH.
#    Keep AirfieldAwareViewTemplateSelector.h because it is a
#    non-XAML custom selector used by generated XAML metadata.
# ------------------------------------------------------------------
$pchText = Get-Content $pch -Raw

$pchText = [regex]::Replace(
    $pchText,
    '(?m)^\s*#include\s+"AirfieldView\.xaml\.h"\s*\r?\n?',
    ''
)

$pchText = [regex]::Replace(
    $pchText,
    '(?m)^\s*#include\s+"AirfieldViewModel\.h"\s*\r?\n?',
    ''
)

Set-Content $pch $pchText -Encoding utf8

# ------------------------------------------------------------------
# 3. Restore AirfieldView.xaml.cpp to the same generated-include
#    pattern already used by TowerView.xaml.cpp.
# ------------------------------------------------------------------
$cppText = Get-Content $cpp -Raw

# Remove direct or guarded occurrences first.
$cppText = [regex]::Replace(
    $cppText,
    '(?ms)\s*#if\s+__has_include\("AirfieldView\.g\.cpp"\)\s*#include\s+"AirfieldView\.g\.cpp"\s*#endif\s*',
    "`r`n"
)

$cppText = [regex]::Replace(
    $cppText,
    '(?m)^\s*#include\s+"AirfieldView\.g\.cpp"\s*\r?\n?',
    ''
)

$needle = '#include "AirfieldView.xaml.h"'
$idx = $cppText.IndexOf($needle)

if ($idx -lt 0) {
    throw 'Could not find #include "AirfieldView.xaml.h" in AirfieldView.xaml.cpp'
}

$insertAt = $idx + $needle.Length
$canonical = @"

#if __has_include("AirfieldView.g.cpp")
#include "AirfieldView.g.cpp"
#endif
"@

$cppText = $cppText.Insert($insertAt, $canonical)

Set-Content $cpp $cppText -Encoding utf8

Write-Host ""
Write-Host "AirfieldView XAML registration normalized."
Write-Host ""
Write-Host "Expected vcxproj entries:"
Write-Host '  <ClInclude Include="AirfieldView.xaml.h"><DependentUpon>AirfieldView.xaml</DependentUpon></ClInclude>'
Write-Host '  <ClCompile Include="AirfieldView.xaml.cpp"><DependentUpon>AirfieldView.xaml</DependentUpon></ClCompile>'
Write-Host '  <Midl Include="AirfieldView.idl"><SubType>Code</SubType><DependentUpon>AirfieldView.xaml</DependentUpon></Midl>'
Write-Host ""
Write-Host "Temporary PCH includes removed:"
Write-Host '  AirfieldView.xaml.h'
Write-Host '  AirfieldViewModel.h'
Write-Host ""
Write-Host "AirfieldView.xaml.cpp now mirrors TowerView generated include pattern."
