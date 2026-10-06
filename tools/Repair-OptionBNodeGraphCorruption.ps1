param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$layout = Join-Path $RepoRoot "xSimAtc.Airfields\airfield_layout.h"
$backupCandidates = @(
    "$layout.before-optionb-nodegraph-v3",
    "$layout.before-optionb-nodegraph-v2",
    "$layout.before-optionb-nodegraph"
)

$backup = $backupCandidates |
    Where-Object { Test-Path $_ } |
    Select-Object -First 1

if (-not (Test-Path $layout))
{
    throw "Missing layout file: $layout"
}

if (-not $backup)
{
    throw "Could not find an Option-B pre-patch backup for airfield_layout.h"
}

Write-Host "Restoring:"
Write-Host "  $layout"
Write-Host "from:"
Write-Host "  $backup"

Copy-Item $backup $layout -Force

# Guard against the exact corruption seen in the compiler log.
$text = Get-Content $layout -Raw

if ($text -match '\$10')
{
    throw "Restore failed: literal `$10 still exists in airfield_layout.h"
}

$fCount = ([regex]::Matches(
    $text,
    '(?m)^\s*(?:inline\s+)?(?:constexpr\s+)?(?:const\s+)?AirfieldNode\s+F\b')).Count

Write-Host ""
Write-Host "Post-restore checks:"
Write-Host "  literal `$10 count: 0"
Write-Host "  named AirfieldNode F declarations: $fCount"

if ($fCount -gt 1)
{
    throw "Restore appears suspicious: more than one named AirfieldNode F declaration remains."
}

Write-Host ""
Write-Host "Option-B node graph source corruption has been rolled back."
Write-Host "The existing 64/64 baseline should be recoverable."
Write-Host ""
Write-Host "Do NOT re-run the v1/v2/v3 Option-B node scripts."
