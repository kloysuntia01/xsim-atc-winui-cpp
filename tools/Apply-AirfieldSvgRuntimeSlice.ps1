param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp",
    [string]$PublishedRoot = "C:\Dev\Xsim.Dev"
)

$ErrorActionPreference = "Stop"

$ui   = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI"
$svg  = Join-Path $ui "Assets\Airfield\airfield-layout.svg"
$xaml = Join-Path $ui "AirfieldView.xaml"
$proj = Join-Path $ui "xSimAtc.Terminal.WinUI.vcxproj"

foreach ($path in @($svg, $xaml, $proj))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

# ------------------------------------------------------------
# 1) Make the XAML source simple and deterministic.
#    WinUI can resolve SVG content directly from ms-appx.
# ------------------------------------------------------------

$xamlText = Get-Content $xaml -Raw

$oldImage = @'
                <Image Width="1000"
                       Height="700"
                       Stretch="Fill">
                    <Image.Source>
                        <SvgImageSource UriSource="ms-appx:///Assets/Airfield/airfield-layout.svg"/>
                    </Image.Source>
                </Image>
'@

$newImage = @'
                <Image x:Name="AirfieldBackground"
                       Width="1000"
                       Height="700"
                       Stretch="Fill"
                       Source="ms-appx:///Assets/Airfield/airfield-layout.svg"/>
'@

if ($xamlText.Contains($oldImage))
{
    $xamlText = $xamlText.Replace($oldImage, $newImage)
}
elseif ($xamlText -notmatch 'x:Name="AirfieldBackground"')
{
    throw "Could not locate the existing Airfield SVG Image block in AirfieldView.xaml"
}

Set-Content $xaml $xamlText -Encoding utf8

# ------------------------------------------------------------
# 2) Normalize the SVG Content item in the vcxproj.
#    DeploymentContent is the important WinUI/MSBuild metadata.
# ------------------------------------------------------------

$projText = Get-Content $proj -Raw

$svgItemPattern =
    '(?ms)\s*<Content\s+Include="Assets\\Airfield\\airfield-layout\.svg"\s*>.*?</Content>\s*'

$svgItem = @'
    <Content Include="Assets\Airfield\airfield-layout.svg">
      <DeploymentContent>true</DeploymentContent>
      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
    </Content>
'@

if ($projText -match $svgItemPattern)
{
    $projText = [regex]::Replace(
        $projText,
        $svgItemPattern,
        "`r`n$svgItem",
        1)
}
else
{
    $itemGroup = @'
  <ItemGroup>
    <Content Include="Assets\Airfield\airfield-layout.svg">
      <DeploymentContent>true</DeploymentContent>
      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
    </Content>
  </ItemGroup>
'@

    $lastClose = $projText.LastIndexOf("</Project>")
    if ($lastClose -lt 0)
    {
        throw "Root </Project> not found in vcxproj"
    }

    $projText =
        $projText.Substring(0, $lastClose) +
        $itemGroup +
        "`r`n" +
        $projText.Substring($lastClose)
}

Set-Content $proj $projText -Encoding utf8

# Validate project XML now, before build.
try
{
    [xml](Get-Content $proj -Raw) | Out-Null
}
catch
{
    throw "xSimAtc.Terminal.WinUI.vcxproj is invalid XML after patch: $($_.Exception.Message)"
}

# ------------------------------------------------------------
# 3) Copy the SVG into the current publish folder immediately.
#    This lets the next run prove the runtime path without waiting
#    on any custom publish target behavior.
# ------------------------------------------------------------

$publishedAssetDir = Join-Path $PublishedRoot "Assets\Airfield"
New-Item -ItemType Directory -Force -Path $publishedAssetDir | Out-Null

$publishedSvg = Join-Path $publishedAssetDir "airfield-layout.svg"
Copy-Item $svg $publishedSvg -Force

if (-not (Test-Path $publishedSvg))
{
    throw "SVG was not copied to published runtime folder: $publishedSvg"
}

# ------------------------------------------------------------
# 4) Print verification.
# ------------------------------------------------------------

Write-Host ""
Write-Host "Airfield SVG runtime slice applied."
Write-Host ""
Write-Host "Source SVG:"
Write-Host "  $svg"
Write-Host ""
Write-Host "Published SVG:"
Write-Host "  $publishedSvg"
Write-Host ""
Write-Host "XAML source:"
Write-Host '  Source="ms-appx:///Assets/Airfield/airfield-layout.svg"'
Write-Host ""
Write-Host "Project metadata:"
Write-Host "  DeploymentContent=true"
Write-Host "  CopyToOutputDirectory=PreserveNewest"
Write-Host ""
Write-Host "No domain behavior changed."
Write-Host "Expected test baseline remains 60/60."
