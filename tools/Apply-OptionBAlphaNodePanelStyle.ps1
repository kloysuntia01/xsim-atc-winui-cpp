param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$view = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $view))
{
    throw "Missing AirfieldView.xaml.cpp: $view"
}

$text = Get-Content $view -Raw

$signature = "void AirfieldView::render_nodes()"
$start = $text.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::render_nodes()."
}

$openBrace = $text.IndexOf("{", $start)

if ($openBrace -lt 0)
{
    throw "Could not find opening brace for render_nodes()."
}

$depth = 0
$closeBrace = -1

for ($i = $openBrace; $i -lt $text.Length; $i++)
{
    $ch = $text[$i]

    if ($ch -eq '{')
    {
        $depth++
    }
    elseif ($ch -eq '}')
    {
        $depth--

        if ($depth -eq 0)
        {
            $closeBrace = $i
            break
        }
    }
}

if ($closeBrace -lt 0)
{
    throw "Could not find closing brace for render_nodes()."
}

$functionText = $text.Substring(
    $start,
    $closeBrace - $start + 1)

$backup = "$view.before-alpha-node-panels-v2"

if (-not (Test-Path $backup))
{
    Copy-Item $view $backup
}

# Exact declaration shape observed in the user's current source:
# Microsoft::UI::Xaml::Controls::Button button;
if ($functionText -notmatch 'Microsoft::UI::Xaml::Controls::Button\s+button\s*;')
{
    throw "Expected node Button declaration not found. No changes made."
}

# Replace the existing visual-state blocks in-place so later code does not
# override a newly inserted default style.

$oldBackground = @'
            button.Background(
                brush(
                    selected
                        ? color(
                            255,
                            133,
                            27)
                        : color(
                            245,
                            196,
                            45)));
'@

$newBackground = @'
            // OPTION_B_ALPHA_NODE_PANEL
            button.Background(
                brush(
                    selected
                        ? color(
                            225,
                            255,
                            133,
                            27)
                        : color(
                            105,
                            18,
                            22,
                            18)));
'@

$oldBorder = @'
            button.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255)
                        : color(
                            30,
                            30,
                            30)));
'@

$newBorder = @'
            button.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            232,
                            140)
                        : color(
                            220,
                            255,
                            193,
                            7)));
'@

$oldThickness = @'
            button.BorderThickness(
                thickness(
                    selected
                        ? 3.0
                        : 2.0));
'@

$newThickness = @'
            button.BorderThickness(
                thickness(
                    selected
                        ? 2.0
                        : 1.25));
'@

$oldForeground = @'
            button.Foreground(
                brush(
                    color(
                        25,
                        25,
                        25)));
'@

$newForeground = @'
            button.Foreground(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255,
                            255)
                        : color(
                            255,
                            255,
                            232,
                            140)));
'@

$replacements = @(
    @{ Old = $oldBackground; New = $newBackground; Name = "background" },
    @{ Old = $oldBorder; New = $newBorder; Name = "border brush" },
    @{ Old = $oldThickness; New = $newThickness; Name = "border thickness" },
    @{ Old = $oldForeground; New = $newForeground; Name = "foreground" }
)

foreach ($replacement in $replacements)
{
    if (-not $functionText.Contains($replacement.Old))
    {
        throw "Could not find expected $($replacement.Name) block. No changes made."
    }

    $functionText = $functionText.Replace(
        $replacement.Old,
        $replacement.New)
}

$text =
    $text.Substring(0, $start) +
    $functionText +
    $text.Substring($closeBrace + 1)

Set-Content $view $text -Encoding utf8

Write-Host ""
Write-Host "Option-B alpha panel style v2 applied."
Write-Host ""
Write-Host "Normal:"
Write-Host "  translucent charcoal fill"
Write-Host "  yellow border"
Write-Host "  warm light text"
Write-Host ""
Write-Host "Selected:"
Write-Host "  translucent orange fill"
Write-Host "  warm bright border"
Write-Host "  white text"
Write-Host ""
Write-Host "No domain behavior changed."
Write-Host "Expected regression baseline remains 72/72."
