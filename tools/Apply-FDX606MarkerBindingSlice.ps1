param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$viewCpp =
    Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

if (-not (Test-Path $viewCpp))
{
    throw "Required file not found: $viewCpp"
}

$cppText = Get-Content $viewCpp -Raw

$backup =
    "$viewCpp.before-fdx606-marker-binding"

if (-not (Test-Path $backup))
{
    Copy-Item $viewCpp $backup
}

# ------------------------------------------------------------
# Locate render_aircraft() exactly.
# ------------------------------------------------------------

$signature =
    "void AirfieldView::render_aircraft()"

$start =
    $cppText.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::render_aircraft()."
}

$openBrace =
    $cppText.IndexOf("{", $start)

if ($openBrace -lt 0)
{
    throw "Could not find render_aircraft() opening brace."
}

$depth = 0
$closeBrace = -1

for ($i = $openBrace; $i -lt $cppText.Length; $i++)
{
    if ($cppText[$i] -eq '{')
    {
        $depth++
    }
    elseif ($cppText[$i] -eq '}')
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
    throw "Could not find render_aircraft() closing brace."
}

$functionText =
    $cppText.Substring(
        $start,
        $closeBrace - $start + 1)

if ($functionText -match 'FDX606_RUNTIME_MARKER_POSITION')
{
    Write-Host "FDX606 marker binding already present."
    exit 0
}

# ------------------------------------------------------------
# Discover the actual aircraft-marker loop.
#
# Expected semantic shape:
#
#   for (const auto& <var> : airfield.aircraft_markers())
#
# The variable name may differ, so discover rather than assume.
# ------------------------------------------------------------

$loopMatch =
    [regex]::Match(
        $functionText,
        'for\s*\(\s*const\s+auto\s*&\s*(?<var>[A-Za-z_][A-Za-z0-9_]*)\s*:\s*[^)]*aircraft_markers\s*\(\s*\)\s*\)')

if (-not $loopMatch.Success)
{
    Write-Host ""
    Write-Host "Could not safely identify the aircraft_markers() loop."
    Write-Host "Current render_aircraft():"
    Write-Host "------------------------------------------------------------"
    Write-Host $functionText
    Write-Host "------------------------------------------------------------"
    throw "Safe patch aborted."
}

$markerVar =
    $loopMatch.Groups["var"].Value

Write-Host "Detected aircraft marker variable: $markerVar"

# Locate the loop braces.
$loopOpen =
    $functionText.IndexOf(
        "{",
        $loopMatch.Index + $loopMatch.Length)

if ($loopOpen -lt 0)
{
    throw "Could not find aircraft marker loop opening brace."
}

$depth = 0
$loopClose = -1

for ($i = $loopOpen; $i -lt $functionText.Length; $i++)
{
    if ($functionText[$i] -eq '{')
    {
        $depth++
    }
    elseif ($functionText[$i] -eq '}')
    {
        $depth--

        if ($depth -eq 0)
        {
            $loopClose = $i
            break
        }
    }
}

if ($loopClose -lt 0)
{
    throw "Could not find aircraft marker loop closing brace."
}

$loopText =
    $functionText.Substring(
        $loopOpen,
        $loopClose - $loopOpen + 1)

# ------------------------------------------------------------
# Verify the real marker shape before modifying anything.
# ------------------------------------------------------------

$callSignPattern =
    '\b' + [regex]::Escape($markerVar) + '\.call_sign\b'

$positionXPattern =
    '\b' + [regex]::Escape($markerVar) + '\.position\.x\b'

$positionYPattern =
    '\b' + [regex]::Escape($markerVar) + '\.position\.y\b'

if (
    $loopText -notmatch $callSignPattern -or
    $loopText -notmatch $positionXPattern -or
    $loopText -notmatch $positionYPattern
)
{
    Write-Host ""
    Write-Host "Aircraft loop found, but marker fields did not match"
    Write-Host "call_sign / position.x / position.y."
    Write-Host "Current loop:"
    Write-Host "------------------------------------------------------------"
    Write-Host $loopText
    Write-Host "------------------------------------------------------------"
    throw "Safe patch aborted."
}

# ------------------------------------------------------------
# Insert one render-position local at the top of the loop.
#
# Only FDX606 is redirected to runtime.position().
# Every other aircraft marker still uses its original position.
# ------------------------------------------------------------

$bindingBlock = @"

        // FDX606_RUNTIME_MARKER_POSITION
        auto render_position =
            $markerVar.position;

        if (
            $markerVar.call_sign == "FDX606" &&
            (
                taxi_ui_runtime_.taxi_runtime().is_running() ||
                taxi_ui_runtime_.taxi_runtime().is_completed()
            )
        )
        {
            render_position =
                taxi_ui_runtime_
                    .taxi_runtime()
                    .position();
        }

"@

$loopText =
    $loopText.Insert(
        1,
        $bindingBlock)

# Replace only marker position reads inside this aircraft loop.
$loopText =
    [regex]::Replace(
        $loopText,
        $positionXPattern,
        'render_position.x')

$loopText =
    [regex]::Replace(
        $loopText,
        $positionYPattern,
        'render_position.y')

# Rebuild function.
$functionText =
    $functionText.Substring(0, $loopOpen) +
    $loopText +
    $functionText.Substring($loopClose + 1)

# Rebuild cpp.
$cppText =
    $cppText.Substring(0, $start) +
    $functionText +
    $cppText.Substring($closeBrace + 1)

Set-Content `
    -Path $viewCpp `
    -Value $cppText `
    -Encoding utf8

Write-Host ""
Write-Host "FDX606 visible marker binding applied."
Write-Host ""
Write-Host "Runtime flow:"
Write-Host "  DispatcherTimer"
Write-Host "  -> AirfieldTaxiRuntime.advance(...)"
Write-Host "  -> render_airfield()"
Write-Host "  -> render_aircraft()"
Write-Host "  -> FDX606 uses runtime.position()"
Write-Host ""
Write-Host "Other aircraft retain their original marker positions."
Write-Host "Expected regression baseline remains 93/93."
