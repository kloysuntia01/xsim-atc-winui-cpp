param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$airfields = Join-Path $RepoRoot "xSimAtc.Airfields"
$testsRoot = Join-Path $RepoRoot "xSimAtc.Airfields.Tests"

$header = Get-ChildItem $airfields -Recurse -Filter "airfield_movement.h" |
    Select-Object -First 1

if (-not $header)
{
    throw "Could not find airfield_movement.h under $airfields"
}

$test = Get-ChildItem $testsRoot -Recurse -File |
    Where-Object {
        $_.Extension -eq ".cpp" -and
        (Select-String -Path $_.FullName -Pattern "AirfieldMovementTests" -Quiet)
    } |
    Select-Object -First 1

if (-not $test)
{
    throw "Could not find AirfieldMovementTests source under $testsRoot"
}

Write-Host "Header: $($header.FullName)"
Write-Host "Tests : $($test.FullName)"

$backup = "$($header.FullName).before-completion-signal"
if (-not (Test-Path $backup))
{
    Copy-Item $header.FullName $backup
}

$text = Get-Content $header.FullName -Raw

# ------------------------------------------------------------
# includes
# ------------------------------------------------------------

if ($text -notmatch '(?m)^\s*#include\s*<functional>')
{
    $text = "#include <functional>`r`n" + $text
}

if ($text -notmatch '(?m)^\s*#include\s*<utility>')
{
    $text = "#include <utility>`r`n" + $text
}

# ------------------------------------------------------------
# public callback setter
# ------------------------------------------------------------

if ($text -notmatch '\bset_completed_callback\s*\(')
{
    $public = [regex]::Match($text, '(?m)^\s*public:\s*$')

    if (-not $public.Success)
    {
        throw "Could not find public: in AirfieldMovement."
    }

    $at = $public.Index + $public.Length

    $addition = @'

        void set_completed_callback(std::function<void()> callback)
        {
            completed_callback_ = std::move(callback);
        }

'@

    $text = $text.Substring(0, $at) + $addition + $text.Substring($at)
}

# ------------------------------------------------------------
# private completion members
# ------------------------------------------------------------

if ($text -notmatch '\bcompleted_callback_\b')
{
    # This branch is unlikely after adding the setter because its body contains
    # the name. Keep it for already-customized source shapes.
    throw "Internal patch ordering error: completed_callback_ not found."
}

if ($text -notmatch 'std::function<void\(\)>\s+completed_callback_')
{
    $lastClassClose = $text.LastIndexOf("};")

    if ($lastClassClose -lt 0)
    {
        throw "Could not find AirfieldMovement class closing brace."
    }

    $members = @'

    private:
        std::function<void()> completed_callback_{};
        bool completion_notified_{ false };

        void notify_completed_once()
        {
            if (completion_notified_)
            {
                return;
            }

            completion_notified_ = true;

            if (completed_callback_)
            {
                completed_callback_();
            }
        }

'@

    $text = $text.Substring(0, $lastClassClose) + $members + $text.Substring($lastClassClose)
}
elseif ($text -notmatch '\bnotify_completed_once\s*\(')
{
    throw "Completion members already exist, but notify_completed_once() does not. Refusing partial patch."
}

# ------------------------------------------------------------
# Re-arm when a new route is assigned.
# ------------------------------------------------------------

if ($text -notmatch 'completion_notified_\s*=\s*false\s*;')
{
    $setRoute = [regex]::Match(
        $text,
        '(?m)(void\s+set_route\s*\([^)]*\)\s*\{)')

    if (-not $setRoute.Success)
    {
        throw "Could not locate AirfieldMovement::set_route(...)."
    }

    $insert = $setRoute.Index + $setRoute.Length

    $text =
        $text.Substring(0, $insert) +
        "`r`n            completion_notified_ = false;" +
        $text.Substring($insert)
}

# ------------------------------------------------------------
# Fire callback at the existing completion transition.
# Support the common completion member names.
# ------------------------------------------------------------

if ($text -notmatch '\bnotify_completed_once\s*\(\s*\)\s*;')
{
    $completionPatterns = @(
        'is_completed_\s*=\s*true\s*;',
        'completed_\s*=\s*true\s*;'
    )

    $patched = $false

    foreach ($pattern in $completionPatterns)
    {
        $m = [regex]::Match($text, $pattern)

        if ($m.Success)
        {
            $replacement =
                $m.Value +
                "`r`n                notify_completed_once();"

            $text =
                $text.Substring(0, $m.Index) +
                $replacement +
                $text.Substring($m.Index + $m.Length)

            $patched = $true
            break
        }
    }

    if (-not $patched)
    {
        throw "Could not find the movement completion assignment (is_completed_=true or completed_=true)."
    }
}

Set-Content $header.FullName $text -Encoding utf8

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------

$testText = Get-Content $test.FullName -Raw

if ($testText -notmatch 'InvokesCompletionCallbackExactlyOnce')
{
    $append = @'

TEST(AirfieldMovementTests, InvokesCompletionCallbackExactlyOnce)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    int completed_count = 0;

    movement.set_completed_callback(
        [&completed_count]()
        {
            ++completed_count;
        });

    movement.set_route(route);
    movement.start();

    movement.advance(1.0);
    movement.advance(1.0);

    EXPECT_TRUE(movement.is_completed());
    EXPECT_EQ(completed_count, 1);
}

TEST(AirfieldMovementTests, NewRouteRearmsCompletionCallback)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    int completed_count = 0;

    movement.set_completed_callback(
        [&completed_count]()
        {
            ++completed_count;
        });

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    EXPECT_EQ(completed_count, 2);
}
'@

    Add-Content $test.FullName $append
}

Write-Host ""
Write-Host "AirfieldMovement completion-signal slice applied."
Write-Host ""
Write-Host "Behavior:"
Write-Host "  movement completion -> callback exactly once"
Write-Host "  assigning a new route re-arms the callback"
Write-Host ""
Write-Host "No Airfields -> Aircrafts dependency added."
Write-Host "Expected baseline from 60 tests: 62."
