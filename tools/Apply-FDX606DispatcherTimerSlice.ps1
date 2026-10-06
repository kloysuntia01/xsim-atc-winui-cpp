param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$viewHeader = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.h"
$viewCpp    = Join-Path $RepoRoot "xSimAtc.Terminal.WinUI\AirfieldView.xaml.cpp"

foreach ($path in @($viewHeader, $viewCpp))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

$headerText = Get-Content $viewHeader -Raw
$cppText    = Get-Content $viewCpp -Raw

$headerBackup = "$viewHeader.before-fdx606-dispatcher-timer"
$cppBackup    = "$viewCpp.before-fdx606-dispatcher-timer"

if (-not (Test-Path $headerBackup))
{
    Copy-Item $viewHeader $headerBackup
}

if (-not (Test-Path $cppBackup))
{
    Copy-Item $viewCpp $cppBackup
}

# ------------------------------------------------------------
# 1) Header: add on_taxi_tick() private method.
# ------------------------------------------------------------

if ($headerText -notmatch 'void\s+on_taxi_tick\s*\(\s*\)\s*;')
{
    $anchor = @'
        void on_node_clicked(
            std::string node_id);
'@

    $replacement = @'
        void on_node_clicked(
            std::string node_id);

        void on_taxi_tick();
'@

    if (-not $headerText.Contains($anchor))
    {
        throw "Could not find on_node_clicked declaration anchor."
    }

    $headerText = $headerText.Replace($anchor, $replacement)
}

# ------------------------------------------------------------
# 2) Header: add DispatcherTimer member next to taxi runtime.
# ------------------------------------------------------------

if ($headerText -notmatch 'DispatcherTimer\s+taxi_timer_')
{
    $anchor = @'
        ::xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime
            taxi_ui_runtime_;
'@

    $replacement = @'
        ::xSimAtc_Terminal_WinUI::AirfieldTaxiUiRuntime
            taxi_ui_runtime_;

        Microsoft::UI::Xaml::DispatcherTimer
            taxi_timer_{ nullptr };
'@

    if (-not $headerText.Contains($anchor))
    {
        throw "Could not find taxi_ui_runtime_ member anchor."
    }

    $headerText = $headerText.Replace($anchor, $replacement)
}

Set-Content $viewHeader $headerText -Encoding utf8

# ------------------------------------------------------------
# 3) Constructor: create timer, set interval, bind Tick.
# ------------------------------------------------------------

$ctorSignature = "AirfieldView::AirfieldView()"
$ctorStart = $cppText.IndexOf($ctorSignature)

if ($ctorStart -lt 0)
{
    throw "Could not find AirfieldView::AirfieldView()."
}

$ctorOpen = $cppText.IndexOf("{", $ctorStart)
if ($ctorOpen -lt 0)
{
    throw "Could not find AirfieldView constructor opening brace."
}

$depth = 0
$ctorClose = -1

for ($i = $ctorOpen; $i -lt $cppText.Length; $i++)
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
            $ctorClose = $i
            break
        }
    }
}

if ($ctorClose -lt 0)
{
    throw "Could not find AirfieldView constructor closing brace."
}

$ctorText = $cppText.Substring(
    $ctorStart,
    $ctorClose - $ctorStart + 1)

if ($ctorText -notmatch 'FDX606_TAXI_TIMER_INIT')
{
    $initBlock = @'

        // FDX606_TAXI_TIMER_INIT
        taxi_timer_ =
            Microsoft::UI::Xaml::DispatcherTimer();

        taxi_timer_.Interval(
            std::chrono::milliseconds{ 50 });

        taxi_timer_.Tick(
            [this](auto&&, auto&&)
            {
                on_taxi_tick();
            });

'@

    $ctorText = $ctorText.Insert(
        $ctorText.Length - 1,
        $initBlock)

    $cppText =
        $cppText.Substring(0, $ctorStart) +
        $ctorText +
        $cppText.Substring($ctorClose + 1)
}

# ------------------------------------------------------------
# 4) on_node_clicked(): start timer after successful runtime start.
# ------------------------------------------------------------

$clickSignature = "void AirfieldView::on_node_clicked"
$clickStart = $cppText.IndexOf($clickSignature)

if ($clickStart -lt 0)
{
    throw "Could not find AirfieldView::on_node_clicked()."
}

$clickOpen = $cppText.IndexOf("{", $clickStart)
$depth = 0
$clickClose = -1

for ($i = $clickOpen; $i -lt $cppText.Length; $i++)
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
            $clickClose = $i
            break
        }
    }
}

if ($clickClose -lt 0)
{
    throw "Could not find on_node_clicked() closing brace."
}

$clickText = $cppText.Substring(
    $clickStart,
    $clickClose - $clickStart + 1)

if ($clickText -notmatch 'FDX606_TAXI_TIMER_START')
{
    $needle = @'
            taxi_ui_runtime_
                .taxi_runtime()
                .load_and_start(
                    std::vector<std::string>{
                        selected_nodes.begin(),
                        selected_nodes.end()
                    });
'@

    if (-not $clickText.Contains($needle))
    {
        throw "Could not find the verified load_and_start block in on_node_clicked()."
    }

    $replacement = @'
            const auto started =
                taxi_ui_runtime_
                    .taxi_runtime()
                    .load_and_start(
                        std::vector<std::string>{
                            selected_nodes.begin(),
                            selected_nodes.end()
                        });

            // FDX606_TAXI_TIMER_START
            if (started)
            {
                taxi_timer_.Start();
            }
'@

    $clickText = $clickText.Replace($needle, $replacement)

    $cppText =
        $cppText.Substring(0, $clickStart) +
        $clickText +
        $cppText.Substring($clickClose + 1)
}

# ------------------------------------------------------------
# 5) Add on_taxi_tick() implementation immediately before
#    screen_x(), which is already a known member in this file.
# ------------------------------------------------------------

if ($cppText -notmatch 'void\s+AirfieldView::on_taxi_tick\s*\(\s*\)')
{
    $screenX = "double AirfieldView::screen_x"
    $insertAt = $cppText.IndexOf($screenX)

    if ($insertAt -lt 0)
    {
        throw "Could not find AirfieldView::screen_x() insertion anchor."
    }

    # Include only runtime advance/re-render in this slice.
    # Marker binding comes next, after this timer compiles cleanly.
    $tickImpl = @'
    void AirfieldView::on_taxi_tick()
    {
        auto& runtime =
            taxi_ui_runtime_.taxi_runtime();

        if (!runtime.is_running())
        {
            taxi_timer_.Stop();
            return;
        }

        runtime.advance(0.04);

        render_airfield();

        if (!runtime.is_running())
        {
            taxi_timer_.Stop();
        }
    }


'@

    $cppText = $cppText.Insert($insertAt, $tickImpl)
}

# ------------------------------------------------------------
# 6) Ensure chrono is available.
# ------------------------------------------------------------

if ($cppText -notmatch '(?m)^#include\s+<chrono>\s*$')
{
    $lastInclude = [regex]::Matches(
        $cppText,
        '(?m)^#include[^\r\n]*') |
        Select-Object -Last 1

    if (-not $lastInclude)
    {
        throw "Could not find include block in AirfieldView.xaml.cpp"
    }

    $insertAt = $lastInclude.Index + $lastInclude.Length
    $cppText = $cppText.Insert(
        $insertAt,
        "`r`n#include <chrono>")
}

Set-Content $viewCpp $cppText -Encoding utf8

Write-Host ""
Write-Host "FDX606 DispatcherTimer slice applied."
Write-Host ""
Write-Host "Behavior:"
Write-Host "  selected route starts AirfieldTaxiRuntime"
Write-Host "  -> DispatcherTimer starts"
Write-Host "  -> every 50 ms runtime.advance(0.04)"
Write-Host "  -> render_airfield()"
Write-Host "  -> timer stops when runtime completes"
Write-Host ""
Write-Host "This slice deliberately does NOT change aircraft marker coordinates yet."
Write-Host "Expected regression baseline remains 93/93."
