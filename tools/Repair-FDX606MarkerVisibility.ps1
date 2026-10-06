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
    "$viewCpp.before-fdx606-marker-visibility-fix"

if (-not (Test-Path $backup))
{
    Copy-Item $viewCpp $backup
}

$old = @'
        if (
            aircraft.call_sign == "FDX606" &&
            (
                taxi_ui_runtime_.taxi_runtime().is_running() ||
                taxi_ui_runtime_.taxi_runtime().is_completed()
            )
        )
'@

$new = @'
        if (
            aircraft.call_sign == "FDX606" &&
            taxi_ui_runtime_
                .taxi_runtime()
                .selected_nodes()
                .size() >= 2 &&
            (
                taxi_ui_runtime_.taxi_runtime().is_running() ||
                taxi_ui_runtime_.taxi_runtime().is_completed()
            )
        )
'@

if ($cppText.Contains($new))
{
    Write-Host "FDX606 visibility guard already applied."
}
elseif ($cppText.Contains($old))
{
    $cppText = $cppText.Replace($old, $new)

    Set-Content `
        -Path $viewCpp `
        -Value $cppText `
        -Encoding utf8

    Write-Host "Applied FDX606 runtime-position visibility guard."
}
else
{
    throw "Expected FDX606 runtime marker condition was not found."
}

Write-Host ""
Write-Host "Behavior:"
Write-Host "  before a taxi route is loaded"
Write-Host "    -> FDX606 uses aircraft.position (layout::F)"
Write-Host ""
Write-Host "  after 2+ route nodes are loaded"
Write-Host "    -> running/completed taxi runtime may supply runtime.position()"
Write-Host ""
Write-Host "Expected regression baseline remains 95/95."
