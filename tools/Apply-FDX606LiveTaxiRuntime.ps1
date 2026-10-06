param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$viewHeader = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.h"
$viewCpp = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

foreach ($path in @($viewHeader, $viewCpp))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

$headerText = Get-Content $viewHeader -Raw
$cppText = Get-Content $viewCpp -Raw

# ------------------------------------------------------------
# 1) Ensure the runtime helper is available in AirfieldView.
#    v1/v2 may already have added these.
# ------------------------------------------------------------

$runtimeInclude = '#include "AirfieldTaxiUiRuntime.h"'

if ($headerText -notmatch [regex]::Escape($runtimeInclude))
{
    $lastInclude = [regex]::Matches(
        $headerText,
        '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    if (-not $lastInclude)
    {
        throw "Could not find include block in AirfieldView.xaml.h"
    }

    $insertAt = $lastInclude.Index + $lastInclude.Length
    $headerText = $headerText.Insert(
        $insertAt,
        "`r`n$runtimeInclude")
}

if ($headerText -notmatch 'AirfieldTaxiUiRuntime\s+taxi_ui_runtime_')
{
    $classClose = $headerText.LastIndexOf("};")

    if ($classClose -lt 0)
    {
        throw "Could not find AirfieldView class end."
    }

    $headerText = $headerText.Insert(
        $classClose,
        "`r`n        xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime taxi_ui_runtime_;`r`n")
}

Set-Content $viewHeader $headerText -Encoding utf8

# ------------------------------------------------------------
# 2) Patch the real on_node_clicked() using the accessor now
#    confirmed from the user's source:
#
#        vm->selected_route_node_ids()
# ------------------------------------------------------------

$signature = "void AirfieldView::on_node_clicked"
$start = $cppText.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::on_node_clicked()."
}

$openBrace = $cppText.IndexOf("{", $start)

if ($openBrace -lt 0)
{
    throw "Could not find on_node_clicked() opening brace."
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
    throw "Could not find on_node_clicked() closing brace."
}

$functionText = $cppText.Substring(
    $start,
    $closeBrace - $start + 1)

if ($functionText -match 'FDX606_TAXI_RUNTIME_BINDING')
{
    Write-Host "FDX606 runtime binding is already present."
    exit 0
}

$selectMatch = [regex]::Match(
    $functionText,
    'vm->select_route_node\(\s*std::move\(node_id\)\s*\)\s*;')

if (-not $selectMatch.Success)
{
    throw "Could not find vm->select_route_node(std::move(node_id));"
}

$insertAt = $selectMatch.Index + $selectMatch.Length

$insertion = @'

        // FDX606_TAXI_RUNTIME_BINDING
        const auto& selected_nodes =
            vm->selected_route_node_ids();

        if (selected_nodes.size() >= 2)
        {
            taxi_ui_runtime_
                .taxi_runtime()
                .load_and_start(
                    std::vector<std::string>{
                        selected_nodes.begin(),
                        selected_nodes.end()
                    });
        }

'@

$functionText = $functionText.Insert(
    $insertAt,
    $insertion)

$cppBackup = "$viewCpp.before-fdx606-runtime-v3"

if (-not (Test-Path $cppBackup))
{
    Copy-Item $viewCpp $cppBackup
}

$cppText =
    $cppText.Substring(0, $start) +
    $functionText +
    $cppText.Substring($closeBrace + 1)

Set-Content $viewCpp $cppText -Encoding utf8

Write-Host ""
Write-Host "FDX606 runtime binding v3 applied."
Write-Host ""
Write-Host "Confirmed accessor:"
Write-Host "  vm->selected_route_node_ids()"
Write-Host ""
Write-Host "Behavior:"
Write-Host "  node click"
Write-Host "  -> selected_route_node_ids()"
Write-Host "  -> AirfieldTaxiRuntime.load_and_start(...)"
Write-Host ""
Write-Host "Marker movement/tick wiring remains deferred."
Write-Host "Expected regression baseline remains 90/90."
