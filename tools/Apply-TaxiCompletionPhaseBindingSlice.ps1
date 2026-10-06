param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$airfields = Join-Path $RepoRoot "xSimAtc.Airfields"
$testsRoot = Join-Path $RepoRoot "xSimAtc.Airfields.Tests"

$movementHeader = Get-ChildItem $airfields -Recurse -Filter "airfield_movement.h" |
    Select-Object -First 1

if (-not $movementHeader)
{
    throw "Could not find airfield_movement.h"
}

$movementTests = Get-ChildItem $testsRoot -Recurse -File |
    Where-Object {
        $_.Extension -eq ".cpp" -and
        (Select-String -Path $_.FullName -Pattern "AirfieldMovementTests" -Quiet)
    } |
    Select-Object -First 1

if (-not $movementTests)
{
    throw "Could not find AirfieldMovementTests source."
}

$movementText = Get-Content $movementHeader.FullName -Raw

$namespaceMatch = [regex]::Match(
    $movementText,
    '(?m)^\s*namespace\s+([A-Za-z_][A-Za-z0-9_:]*)\s*\{')

if (-not $namespaceMatch.Success)
{
    throw "Could not determine AirfieldMovement namespace."
}

$namespaceName = $namespaceMatch.Groups[1].Value
$bindingHeader = Join-Path $movementHeader.Directory.FullName "airfield_movement_completion_binding.h"

$binding = @"
#pragma once

#include "airfield_movement.h"

namespace $namespaceName
{
    template<typename TAircraft, typename TPhase>
    void bind_movement_completion_to_phase(
        AirfieldMovement& movement,
        TAircraft& aircraft,
        TPhase phase)
    {
        movement.set_completed_callback(
            [&aircraft, phase]()
            {
                aircraft.request_transition(phase);
            });
    }
}
"@

Set-Content $bindingHeader $binding -Encoding utf8

$testText = Get-Content $movementTests.FullName -Raw

# The tests already resolve airfield_movement.h from this same include directory,
# so use the sibling header name directly. This avoids .NET Framework versions
# of PowerShell that do not provide System.IO.Path::GetRelativePath().
$includeLine = '#include "airfield_movement_completion_binding.h"'

if ($testText -notmatch [regex]::Escape($includeLine))
{
    $includeMatches = [regex]::Matches(
        $testText,
        '(?m)^\s*#include[^\r\n]*')

    if ($includeMatches.Count -eq 0)
    {
        $testText = $includeLine + "`r`n" + $testText
    }
    else
    {
        $last = $includeMatches[$includeMatches.Count - 1]
        $at = $last.Index + $last.Length

        $testText =
            $testText.Substring(0, $at) +
            "`r`n" +
            $includeLine +
            $testText.Substring($at)
    }
}

if ($testText -notmatch 'BindsCompletionToRequestedPhase')
{
    # Insert before the original file's final namespace-closing brace,
    # preserving the scope that already makes AirfieldMovement/Waypoint visible.
    $lastClose = $testText.LastIndexOf("}")

    if ($lastClose -lt 0)
    {
        throw "Could not locate final namespace closing brace in test source."
    }

    $tests = @'

namespace
{
    enum class FakeAircraftPhase
    {
        Ready,
        InPosition
    };

    struct FakeAircraft final
    {
        FakeAircraftPhase phase{ FakeAircraftPhase::Ready };
        int transition_requests{};

        bool request_transition(FakeAircraftPhase requested)
        {
            ++transition_requests;
            phase = requested;
            return true;
        }
    };
}

TEST(AirfieldMovementTests, BindsCompletionToRequestedPhase)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    FakeAircraft aircraft;

    bind_movement_completion_to_phase(
        movement,
        aircraft,
        FakeAircraftPhase::InPosition);

    movement.set_route(route);
    movement.start();
    movement.advance(1.0);

    EXPECT_TRUE(movement.is_completed());
    EXPECT_EQ(aircraft.phase, FakeAircraftPhase::InPosition);
    EXPECT_EQ(aircraft.transition_requests, 1);
}

TEST(AirfieldMovementTests, DoesNotRequestPhaseBeforeMovementCompletes)
{
    const std::vector<AirfieldWaypoint> route{
        { "A", { 0.0, 0.0 } },
        { "B", { 1.0, 0.0 } }
    };

    AirfieldMovement movement;
    FakeAircraft aircraft;

    bind_movement_completion_to_phase(
        movement,
        aircraft,
        FakeAircraftPhase::InPosition);

    movement.set_route(route);
    movement.start();
    movement.advance(0.5);

    EXPECT_FALSE(movement.is_completed());
    EXPECT_EQ(aircraft.phase, FakeAircraftPhase::Ready);
    EXPECT_EQ(aircraft.transition_requests, 0);
}

'@

    $testText =
        $testText.Substring(0, $lastClose) +
        $tests +
        $testText.Substring($lastClose)
}

Set-Content $movementTests.FullName $testText -Encoding utf8

Write-Host ""
Write-Host "Taxi completion phase-binding slice v2 applied."
Write-Host ""
Write-Host "Compatibility fix:"
Write-Host "  removed System.IO.Path::GetRelativePath()"
Write-Host "  uses sibling include: airfield_movement_completion_binding.h"
Write-Host ""
Write-Host "Expected baseline from 62 tests: 64."
