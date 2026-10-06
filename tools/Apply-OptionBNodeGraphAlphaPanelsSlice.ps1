param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$searchRoots = @(
    (Join-Path $RepoRoot "xSimAtc.Airfields"),
    (Join-Path $RepoRoot "xSimAtc.Terminal.WinUI")
)

$sourceFiles = foreach ($root in $searchRoots)
{
    if (Test-Path $root)
    {
        Get-ChildItem $root -Recurse -File |
            Where-Object { $_.Extension -in @(".h", ".hpp", ".cpp") }
    }
}

# Find the authoritative file that actually contains the F/H/J/18L node literals.
$candidates = foreach ($file in $sourceFiles)
{
    $text = Get-Content $file.FullName -Raw

    if ($text -match '"F"' -and
        $text -match '"H"' -and
        $text -match '"J"' -and
        $text -match '"18L"' -and
        $text -match 'AirfieldNode')
    {
        [pscustomobject]@{
            File = $file
            Text = $text
        }
    }
}

if (-not $candidates)
{
    Write-Host ""
    Write-Host "Could not find the authoritative F/H/J/18L node-definition file."
    Write-Host "Searching for any source files containing these literals..."
    Write-Host ""

    foreach ($file in $sourceFiles)
    {
        $hits = Select-String -Path $file.FullName -Pattern '"F"|"H"|"J"|"18L"' -SimpleMatch:$false
        if ($hits)
        {
            Write-Host $file.FullName
            $hits | Select-Object -First 8 | ForEach-Object {
                Write-Host ("  {0}: {1}" -f $_.LineNumber, $_.Line.Trim())
            }
        }
    }

    throw "Unable to locate F/H/J/18L AirfieldNode definitions automatically."
}

# Prefer the Airfields domain over WinUI if more than one file matches.
$chosen = $candidates |
    Sort-Object @{
        Expression = {
            if ($_.File.FullName -like "*xSimAtc.Airfields*") { 0 } else { 1 }
        }
    }, @{
        Expression = { $_.File.FullName }
    } |
    Select-Object -First 1

$nodeFile = $chosen.File.FullName
$text = $chosen.Text

Write-Host "Authoritative node file:"
Write-Host "  $nodeFile"

$backup = "$nodeFile.before-optionb-nodegraph-v3"
if (-not (Test-Path $backup))
{
    Copy-Item $nodeFile $backup
}

# Match an F node expression without assuming line layout or specific enum spacing.
$fPattern = '(?ms)(?<expr>(?:AirfieldNode\s*)?\{\s*"F"\s*,\s*AirfieldNodeKind::(?<kind>[A-Za-z_][A-Za-z0-9_]*)\s*,\s*\{\s*(?<x>[0-9.]+)\s*,\s*(?<y>[0-9.]+)\s*\}\s*\})'
$fMatch = [regex]::Match($text, $fPattern)

if (-not $fMatch.Success)
{
    Write-Host ""
    Write-Host "Found the right file, but could not parse node F."
    Write-Host "Nearby source:"
    Select-String -Path $nodeFile -Pattern '"F"' -Context 6,6 |
        ForEach-Object { Write-Host $_.Context.PreContext; Write-Host $_.Line; Write-Host $_.Context.PostContext }

    throw "Could not parse the F AirfieldNode expression."
}

$fExpr = $fMatch.Groups["expr"].Value
$kind = $fMatch.Groups["kind"].Value

Write-Host "Detected F node kind: $kind"

function Clone-NodeExpression(
    [string]$Template,
    [string]$Id,
    [double]$X,
    [double]$Y)
{
    $xs = $X.ToString(
        "0.000",
        [System.Globalization.CultureInfo]::InvariantCulture)

    $ys = $Y.ToString(
        "0.000",
        [System.Globalization.CultureInfo]::InvariantCulture)

    $result = [regex]::Replace(
        $Template,
        '"F"',
        '"' + $Id + '"',
        1)

    $result = [regex]::Replace(
        $result,
        '(AirfieldNodeKind::[A-Za-z_][A-Za-z0-9_]*\s*,\s*\{\s*)[0-9.]+(\s*,\s*)[0-9.]+(\s*\})',
        '$1' + $xs + '$2' + $ys + '$3',
        1)

    return $result
}

if ($text -notmatch '"B"\s*,\s*AirfieldNodeKind::')
{
    $nodes = @(
        @{ Id = "B"; X = 0.120; Y = 0.164 },
        @{ Id = "C"; X = 0.250; Y = 0.164 },
        @{ Id = "D"; X = 0.750; Y = 0.164 },
        @{ Id = "E"; X = 0.390; Y = 0.329 },
        @{ Id = "G"; X = 0.610; Y = 0.329 },
        @{ Id = "K"; X = 0.820; Y = 0.329 },
        @{ Id = "L"; X = 0.250; Y = 0.729 },
        @{ Id = "M"; X = 0.400; Y = 0.729 },
        @{ Id = "N"; X = 0.600; Y = 0.729 },
        @{ Id = "P"; X = 0.180; Y = 0.729 },
        @{ Id = "Q"; X = 0.400; Y = 0.779 },
        @{ Id = "R"; X = 0.600; Y = 0.779 },
        @{ Id = "S"; X = 0.750; Y = 0.729 }
    )

    $clones = foreach ($node in $nodes)
    {
        Clone-NodeExpression `
            -Template $fExpr `
            -Id $node.Id `
            -X $node.X `
            -Y $node.Y
    }

    # Inspect the F statement to decide whether this is a per-node statement or
    # a larger initializer list.
    $exprStart = $fMatch.Index
    $exprEnd = $fMatch.Index + $fMatch.Length
    $prevSemi = $text.LastIndexOf(";", $exprStart)
    $nextSemi = $text.IndexOf(";", $exprEnd)

    if ($nextSemi -ge 0)
    {
        $stmtStart = if ($prevSemi -ge 0) { $prevSemi + 1 } else { 0 }
        $statement = $text.Substring(
            $stmtStart,
            $nextSemi - $stmtStart + 1)

        $nodeCount = [regex]::Matches(
            $statement,
            'AirfieldNodeKind::').Count
    }
    else
    {
        $statement = ""
        $nodeCount = 99
    }

    if ($statement.Length -lt 800 -and $nodeCount -eq 1)
    {
        $statements = foreach ($clone in $clones)
        {
            $statement.Replace($fExpr, $clone).Trim()
        }

        $insert = ($statements -join "`r`n") + "`r`n"

        $text =
            $text.Substring(0, $stmtStart) +
            "`r`n" +
            $insert +
            $text.Substring($stmtStart)

        Write-Host "Inserted nodes using cloned per-node statements."
    }
    else
    {
        $insert = ($clones -join ",`r`n            ") + ",`r`n            "

        $text =
            $text.Substring(0, $exprStart) +
            $insert +
            $text.Substring($exprStart)

        Write-Host "Inserted nodes into the existing node initializer."
    }

    Set-Content $nodeFile $text -Encoding utf8
}
else
{
    Write-Host "Option-B nodes already present in authoritative source."
}

# ------------------------------------------------------------
# Alpha-panel styling in the live WinUI node renderer.
# ------------------------------------------------------------

$view = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $view))
{
    throw "Missing AirfieldView.xaml.cpp"
}

$viewBackup = "$view.before-optionb-nodegraph-v3"
if (-not (Test-Path $viewBackup))
{
    Copy-Item $view $viewBackup
}

$viewText = Get-Content $view -Raw
$startSig = "void AirfieldView::render_nodes()"
$start = $viewText.IndexOf($startSig)

if ($start -lt 0)
{
    throw "Could not find render_nodes()."
}

$tail = $viewText.Substring($start + $startSig.Length)
$nextMatch = [regex]::Match(
    $tail,
    '(?m)^\s*void\s+AirfieldView::[A-Za-z_][A-Za-z0-9_]*\s*\(')

if (-not $nextMatch.Success)
{
    throw "Could not determine render_nodes() end."
}

$next = $start + $startSig.Length + $nextMatch.Index

$before = $viewText.Substring(0, $start)
$block = $viewText.Substring($start, $next - $start)
$after = $viewText.Substring($next)

$block = [regex]::Replace(
    $block,
    'button\.Width\(\s*[0-9.]+\s*\);',
    'button.Width(36.0);')

$block = [regex]::Replace(
    $block,
    'button\.Height\(\s*[0-9.]+\s*\);',
    'button.Height(28.0);')

$block = [regex]::Replace(
    $block,
    'button\.CornerRadius\(\s*Microsoft::UI::Xaml::CornerRadius\{[^}]*\}\s*\);',
    'button.CornerRadius(Microsoft::UI::Xaml::CornerRadius{ 5.0 });')

$block = [regex]::Replace(
    $block,
    'color\(\s*255\s*,\s*255\s*,\s*193\s*,\s*7\s*\)',
    'color(125, 12, 18, 12)')

$viewText = $before + $block + $after
Set-Content $view $viewText -Encoding utf8

Write-Host ""
Write-Host "Option-B node graph v3 applied."
Write-Host "Added intersection nodes: B C D E G K L M N P Q R S"
Write-Host "Existing nodes retained: F H J 18L"
Write-Host "Node controls updated toward compact alpha panels."
Write-Host "Expected regression baseline remains 64/64."
