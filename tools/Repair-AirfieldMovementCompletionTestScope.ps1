param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$testsRoot = Join-Path $RepoRoot "xSimAtc.Airfields.Tests"

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

Write-Host "Repairing: $($test.FullName)"

$backup = "$($test.FullName).before-completion-test-scope-fix"

if (-not (Test-Path $backup))
{
    Copy-Item $test.FullName $backup
}

$text = Get-Content $test.FullName -Raw

$marker = "TEST(AirfieldMovementTests, InvokesCompletionCallbackExactlyOnce)"
$markerIndex = $text.IndexOf($marker)

if ($markerIndex -lt 0)
{
    throw "Could not find the appended completion callback tests."
}

# Remove the broken appended tests from their current EOF position.
$original = $text.Substring(0, $markerIndex).TrimEnd()

# The existing test file already compiled at the 60/60 checkpoint.
# Its unqualified AirfieldWaypoint/AirfieldMovement names therefore live
# in the file's original scope (typically an anonymous namespace).
# Reinsert the new tests just before that original scope closes.
$namespaceMatch = [regex]::Match($original, '(?m)^\s*namespace(?:\s+\w+(?:::\w+)*)?\s*\{')

if (-not $namespaceMatch.Success)
{
    throw "No namespace scope found in the original test file; refusing to guess."
}

$lastClose = $original.LastIndexOf("}")

if ($lastClose -lt 0 -or $lastClose -le $namespaceMatch.Index)
{
    throw "Could not identify the original namespace closing brace."
}

$tests = @'

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

$fixed =
    $original.Substring(0, $lastClose) +
    $tests +
    $original.Substring($lastClose) +
    "`r`n"

Set-Content $test.FullName $fixed -Encoding utf8

Write-Host ""
Write-Host "Completion callback tests moved into the original test namespace."
Write-Host "No production code changed."
Write-Host "Expected total remains 62 tests after the completion-signal slice."
