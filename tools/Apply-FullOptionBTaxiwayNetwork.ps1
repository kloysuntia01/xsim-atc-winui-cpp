param(
    [string]$RepoRoot = "C:\Dev\xsimatc-cpp"
)

$ErrorActionPreference = "Stop"

$layout = Join-Path $RepoRoot "xSimAtc.Airfields\airfield_taxiway_layout.h"
$overlayTests = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\airfield_taxiway_overlay_tests.cpp"
$testsCmake = Join-Path $RepoRoot "xSimAtc.Airfields.Tests\CMakeLists.txt"

foreach ($path in @($layout, $overlayTests, $testsCmake))
{
    if (-not (Test-Path $path))
    {
        throw "Required file not found: $path"
    }
}

$layoutBackup = "$layout.before-full-optionb-network"
$overlayBackup = "$overlayTests.before-full-optionb-network"

if (-not (Test-Path $layoutBackup))
{
    Copy-Item $layout $layoutBackup
}

if (-not (Test-Path $overlayBackup))
{
    Copy-Item $overlayTests $overlayBackup
}

$layoutText = Get-Content $layout -Raw

if ($layoutText -match 'TaxiwayNode\{\s*"B"')
{
    Write-Host "Extended Option-B nodes already appear to be present."
}
else
{
    $oldNodes = @'
    inline constexpr std::array<TaxiwayNode, 7> Nodes{
        TaxiwayNode{ "F",   { 0.635, 0.355 } },
        TaxiwayNode{ "F1",  { 0.595, 0.520 } },
        TaxiwayNode{ "H",   { 0.570, 0.620 } },
        TaxiwayNode{ "H1",  { 0.470, 0.690 } },
        TaxiwayNode{ "J",   { 0.395, 0.735 } },
        TaxiwayNode{ "J1",  { 0.360, 0.835 } },
        TaxiwayNode{ "18L", { 0.350, 0.895 } }
    };
'@

    $newNodes = @'
    inline constexpr std::array<TaxiwayNode, 20> Nodes{
        // Existing controller / primary-route nodes.
        TaxiwayNode{ "F",   { 0.635, 0.355 } },
        TaxiwayNode{ "F1",  { 0.595, 0.520 } },
        TaxiwayNode{ "H",   { 0.570, 0.620 } },
        TaxiwayNode{ "H1",  { 0.470, 0.690 } },
        TaxiwayNode{ "J",   { 0.395, 0.735 } },
        TaxiwayNode{ "J1",  { 0.360, 0.835 } },
        TaxiwayNode{ "18L", { 0.350, 0.895 } },

        // Extended Option-B taxiway turns / intersections.
        TaxiwayNode{ "B",   { 0.120, 0.164 } },
        TaxiwayNode{ "C",   { 0.250, 0.164 } },
        TaxiwayNode{ "D",   { 0.750, 0.164 } },
        TaxiwayNode{ "E",   { 0.390, 0.329 } },
        TaxiwayNode{ "G",   { 0.610, 0.329 } },
        TaxiwayNode{ "K",   { 0.820, 0.329 } },
        TaxiwayNode{ "L",   { 0.250, 0.729 } },
        TaxiwayNode{ "M",   { 0.400, 0.729 } },
        TaxiwayNode{ "N",   { 0.600, 0.729 } },
        TaxiwayNode{ "P",   { 0.180, 0.729 } },
        TaxiwayNode{ "Q",   { 0.400, 0.779 } },
        TaxiwayNode{ "R",   { 0.600, 0.779 } },
        TaxiwayNode{ "S",   { 0.750, 0.729 } }
    };
'@

    if (-not $layoutText.Contains($oldNodes))
    {
        throw "The expected 7-node taxiway layout block was not found. No layout changes made."
    }

    $layoutText = $layoutText.Replace(
        $oldNodes,
        $newNodes)

    $oldGraph = @'
        graph.add_edge("F", "F1");
        graph.add_edge("F1", "H");
        graph.add_edge("H", "H1");
        graph.add_edge("H1", "J");
        graph.add_edge("J", "J1");
        graph.add_edge("J1", "18L");
'@

    $newGraph = @'
        // Primary controller route.
        graph.add_edge("F", "F1");
        graph.add_edge("F1", "H");
        graph.add_edge("H", "H1");
        graph.add_edge("H1", "J");
        graph.add_edge("J", "J1");
        graph.add_edge("J1", "18L");

        // Upper taxiway network.
        graph.add_edge("B", "C");
        graph.add_edge("C", "E");
        graph.add_edge("E", "G");
        graph.add_edge("G", "K");
        graph.add_edge("K", "D");

        // Tie the upper network into the primary route.
        graph.add_edge("G", "F");
        graph.add_edge("E", "H1");

        // Lower taxiway network.
        graph.add_edge("P", "L");
        graph.add_edge("L", "M");
        graph.add_edge("M", "N");
        graph.add_edge("N", "S");

        graph.add_edge("M", "Q");
        graph.add_edge("Q", "J1");

        graph.add_edge("N", "R");
        graph.add_edge("R", "S");

        // Vertical/perimeter connectors.
        graph.add_edge("C", "L");
        graph.add_edge("D", "S");

        // Connect the lower network to the controller route.
        graph.add_edge("M", "J");
        graph.add_edge("N", "H");
'@

    if (-not $layoutText.Contains($oldGraph))
    {
        throw "The expected primary build_graph() block was not found. No layout changes made."
    }

    $layoutText = $layoutText.Replace(
        $oldGraph,
        $newGraph)

    Set-Content $layout $layoutText -Encoding utf8

    Write-Host "Extended Option-B taxiway graph added."
}

# Update the overlay expectation from exactly 3 intermediates to all 16
# non-controller taxiway nodes.
$overlayText = Get-Content $overlayTests -Raw

$oldOverlayTest = @'
        ASSERT_EQ(nodes.size(), 3u);

        EXPECT_EQ(nodes[0]->id, "F1");
        EXPECT_EQ(nodes[1]->id, "H1");
        EXPECT_EQ(nodes[2]->id, "J1");
'@

$newOverlayTest = @'
        ASSERT_EQ(nodes.size(), 16u);

        const auto contains =
            [&nodes](std::string_view id)
            {
                for (const auto* node : nodes)
                {
                    if (node->id == id)
                    {
                        return true;
                    }
                }

                return false;
            };

        for (const auto* id : {
            "F1", "H1", "J1",
            "B", "C", "D", "E", "G", "K",
            "L", "M", "N", "P", "Q", "R", "S" })
        {
            EXPECT_TRUE(contains(id))
                << "Missing intermediate overlay node " << id;
        }
'@

if ($overlayText.Contains($oldOverlayTest))
{
    $overlayText = $overlayText.Replace(
        $oldOverlayTest,
        $newOverlayTest)

    Set-Content $overlayTests $overlayText -Encoding utf8
    Write-Host "Updated intermediate-overlay test for the extended graph."
}
elseif ($overlayText -match 'ASSERT_EQ\(nodes\.size\(\),\s*16u\)')
{
    Write-Host "Overlay test already expects the extended graph."
}
else
{
    throw "Could not identify the existing overlay intermediate-node expectation."
}

$cmakeText = Get-Content $testsCmake -Raw

if ($cmakeText -notmatch 'airfield_full_taxiway_network_tests\.cpp')
{
    $append = @'

# Full Option-B taxiway network
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_full_taxiway_network_tests.cpp
)
'@

    Add-Content -Path $testsCmake -Value $append -Encoding utf8
    Write-Host "Registered airfield_full_taxiway_network_tests.cpp."
}
else
{
    Write-Host "Full-network tests already registered."
}

Write-Host ""
Write-Host "Full Option-B taxiway network slice applied."
Write-Host ""
Write-Host "New visible intermediate alpha-panel nodes:"
Write-Host "  B C D E G K L M N P Q R S"
Write-Host ""
Write-Host "Existing intermediate nodes remain:"
Write-Host "  F1 H1 J1"
Write-Host ""
Write-Host "Interactive controller nodes remain:"
Write-Host "  F H J 18L"
Write-Host ""
Write-Host "Because render_nodes() already uses taxiway_overlay::intermediate_nodes(),"
Write-Host "the new graph nodes will appear automatically as alpha panels."
Write-Host ""
Write-Host "Expected test total after configure/build: 78."
