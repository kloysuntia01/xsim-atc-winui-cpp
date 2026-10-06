param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$ui = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI"
$proj = Join-Path $ui "xSimAtc.Terminal.WinUI.vcxproj"

if (-not (Test-Path $proj))
{
    throw "Project file not found: $proj"
}

$backup = "$proj.before-projectreference-repair"

if (-not (Test-Path $backup))
{
    Copy-Item $proj $backup
}

$text = Get-Content $proj -Raw

# ------------------------------------------------------------
# Repair the two known ProjectReference metadata blocks.
# The previous SVG registration script accidentally replaced
# every </Project> occurrence, including the nested metadata
# elements inside ProjectReference.
# ------------------------------------------------------------

$communicationsPattern =
    '(?ms)<ProjectReference\s+Include="\.\.\\xSimAtc\.Communications\\xSimAtc\.Communications\.vcxproj"\s*>.*?</ProjectReference>'

$communicationsReplacement = @'
<ProjectReference Include="..\xSimAtc.Communications\xSimAtc.Communications.vcxproj">
      <Project>{a2fc6a58-198c-4e43-85ed-67b35db7d3d5}</Project>
    </ProjectReference>
'@

$aircraftsPattern =
    '(?ms)<ProjectReference\s+Include="\.\.\\xSimAtc\.Aircrafts\\xSimAtc\.Aircrafts\.vcxproj"\s*>.*?</ProjectReference>'

$aircraftsReplacement = @'
<ProjectReference Include="..\xSimAtc.Aircrafts\xSimAtc.Aircrafts.vcxproj">
      <Project>{1ed9a9b3-1aca-4796-ba1f-b28a540c0de1}</Project>
    </ProjectReference>
'@

if ($text -notmatch $communicationsPattern)
{
    throw "Could not locate Communications ProjectReference."
}

if ($text -notmatch $aircraftsPattern)
{
    throw "Could not locate Aircrafts ProjectReference."
}

$text = [regex]::Replace(
    $text,
    $communicationsPattern,
    $communicationsReplacement,
    1)

$text = [regex]::Replace(
    $text,
    $aircraftsPattern,
    $aircraftsReplacement,
    1)

# ------------------------------------------------------------
# Remove every prior SVG registration block, including copies
# that may have been inserted inside metadata elements.
# ------------------------------------------------------------

$svgItemGroupPattern = @'
(?ms)\s*<ItemGroup>\s*<Content\s+Include="Assets\\Airfield\\airfield-layout\.svg"\s*>\s*<CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>\s*</Content>\s*</ItemGroup>\s*
'@

$text = [regex]::Replace(
    $text,
    $svgItemGroupPattern,
    "`r`n")

# ------------------------------------------------------------
# Re-add ONE SVG Content item immediately before the ROOT
# closing </Project>. Use LastIndexOf so nested metadata tags
# are never touched again.
# ------------------------------------------------------------

$svgItemGroup = @'
  <ItemGroup>
    <Content Include="Assets\Airfield\airfield-layout.svg">
      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
    </Content>
  </ItemGroup>
'@

$lastClose = $text.LastIndexOf("</Project>")

if ($lastClose -lt 0)
{
    throw "Root </Project> closing tag not found."
}

$text =
    $text.Substring(0, $lastClose) +
    $svgItemGroup +
    "`r`n" +
    $text.Substring($lastClose)

Set-Content $proj $text -Encoding utf8

# ------------------------------------------------------------
# Validate the repaired XML and the exact Project GUID values.
# ------------------------------------------------------------

try
{
    [xml]$xml = Get-Content $proj -Raw
}
catch
{
    throw "Repaired vcxproj is not valid XML: $($_.Exception.Message)"
}

$refs = @(
    @{
        Include = "..\xSimAtc.Communications\xSimAtc.Communications.vcxproj"
        Guid = "{a2fc6a58-198c-4e43-85ed-67b35db7d3d5}"
    },
    @{
        Include = "..\xSimAtc.Aircrafts\xSimAtc.Aircrafts.vcxproj"
        Guid = "{1ed9a9b3-1aca-4796-ba1f-b28a540c0de1}"
    }
)

$ns = New-Object System.Xml.XmlNamespaceManager($xml.NameTable)
$ns.AddNamespace("msb", $xml.Project.NamespaceURI)

foreach ($expected in $refs)
{
    $node = $xml.SelectSingleNode(
        "//msb:ProjectReference[@Include='$($expected.Include)']",
        $ns)

    if ($null -eq $node)
    {
        throw "Missing ProjectReference: $($expected.Include)"
    }

    $projectValue = $node.Project

    if ($projectValue -ne $expected.Guid)
    {
        throw "Invalid Project GUID for $($expected.Include): $projectValue"
    }

    Write-Host "OK  $($expected.Include)"
    Write-Host "    $projectValue"
}

Write-Host ""
Write-Host "ProjectReference metadata repaired."
Write-Host "SVG asset registration normalized to one root ItemGroup."
