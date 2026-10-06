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
    "$viewCpp.before-render-nodes-controller-repair"

if (-not (Test-Path $backup))
{
    Copy-Item $viewCpp $backup
}

$signature = "void AirfieldView::render_nodes()"
$start = $cppText.IndexOf($signature)

if ($start -lt 0)
{
    throw "Could not find AirfieldView::render_nodes()."
}

$openBrace = $cppText.IndexOf("{", $start)

if ($openBrace -lt 0)
{
    throw "Could not find render_nodes() opening brace."
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
    throw "Could not find render_nodes() closing brace."
}

$currentFunction =
    $cppText.Substring(
        $start,
        $closeBrace - $start + 1)

if ($currentFunction -match 'OPTION_B_CONTROLLER_NODE_REPAIR')
{
    Write-Host "render_nodes() controller repair already applied."
    exit 0
}

$replacement = @'
void AirfieldView::render_nodes()
    {
        const auto vm =
            winrt::get_self<
                winrt::xSimAtc_Terminal_WinUI::
                    implementation::AirfieldViewModel>(
                        view_model_);

        const auto& airfield =
            vm->airfield();

        // OPTION_B_INTERACTIVE_TAXIWAY_PANELS
        //
        // Render physical/intermediate taxiway nodes first so the larger
        // controller decision nodes appended below remain on top.
        for (const auto* taxiway_node :
             xsim::airfields::taxiway_overlay::intermediate_nodes())
        {
            Microsoft::UI::Xaml::Controls::Button panel;

            panel.Width(30.0);
            panel.Height(22.0);
            panel.Padding(thickness(0.0));

            panel.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{
                    4.0
                });

            const bool selected =
                vm->is_route_node_selected(
                    std::string{
                        taxiway_node->id
                    });

            panel.Background(
                brush(
                    selected
                        ? color(
                            210,
                            255,
                            133,
                            27)
                        : color(
                            90,
                            14,
                            20,
                            16)));

            panel.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            232,
                            140)
                        : color(
                            190,
                            255,
                            193,
                            7)));

            panel.BorderThickness(
                thickness(
                    selected
                        ? 2.0
                        : 1.0));

            panel.Foreground(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255,
                            255)
                        : color(
                            230,
                            255,
                            232,
                            140)));

            panel.FontSize(10.0);

            panel.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            panel.Content(
                winrt::box_value(
                    winrt::to_hstring(
                        taxiway_node->id)));

            const auto node_id =
                std::string{
                    taxiway_node->id
                };

            panel.Click(
                [this, node_id](
                    auto&&,
                    auto&&)
                {
                    on_node_clicked(
                        node_id);
                });

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    panel,
                    screen_x(
                        taxiway_node->position.x) -
                        15.0);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    panel,
                    screen_y(
                        taxiway_node->position.y) -
                        11.0);

            AirfieldCanvas().Children().Append(
                panel);
        }

        // OPTION_B_CONTROLLER_NODE_REPAIR
        //
        // F / H / J / 18L are ATC decision nodes. They disappeared when
        // render_nodes() was reduced to intermediate-node loops only.
        // Render them last, larger, and slightly offset from nearby
        // physical waypoint labels. Domain coordinates are unchanged.
        for (const auto& node :
             airfield.nodes())
        {
            Microsoft::UI::Xaml::Controls::Button button;

            button.Width(52.0);
            button.Height(34.0);
            button.Padding(thickness(0.0));

            button.CornerRadius(
                Microsoft::UI::Xaml::CornerRadius{
                    6.0
                });

            const bool selected =
                vm->is_route_node_selected(
                    node.id);

            button.Background(
                brush(
                    selected
                        ? color(
                            225,
                            255,
                            133,
                            27)
                        : color(
                            125,
                            18,
                            22,
                            18)));

            button.BorderBrush(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            245,
                            180)
                        : color(
                            235,
                            255,
                            193,
                            7)));

            button.BorderThickness(
                thickness(
                    selected
                        ? 3.0
                        : 2.0));

            button.Foreground(
                brush(
                    selected
                        ? color(
                            255,
                            255,
                            255,
                            255)
                        : color(
                            255,
                            255,
                            240,
                            180)));

            button.FontSize(12.0);

            button.FontWeight(
                Microsoft::UI::Text::
                    FontWeights::SemiBold());

            button.Content(
                winrt::box_value(
                    winrt::to_hstring(
                        node.id)));

            const auto node_id =
                node.id;

            button.Click(
                [this, node_id](
                    auto&&,
                    auto&&)
                {
                    on_node_clicked(
                        node_id);
                });

            double label_offset_x =
                -26.0;

            double label_offset_y =
                -17.0;

            if (node.id == "F")
            {
                label_offset_x = -26.0;
                label_offset_y = 12.0;
            }
            else if (node.id == "H")
            {
                label_offset_x = -26.0;
                label_offset_y = -48.0;
            }
            else if (node.id == "J")
            {
                label_offset_x = -62.0;
                label_offset_y = -8.0;
            }
            else if (node.id == "18L")
            {
                label_offset_x = -62.0;
                label_offset_y = -8.0;
            }

            Microsoft::UI::Xaml::Controls::Canvas::
                SetLeft(
                    button,
                    screen_x(
                        node.position.x) +
                        label_offset_x);

            Microsoft::UI::Xaml::Controls::Canvas::
                SetTop(
                    button,
                    screen_y(
                        node.position.y) +
                        label_offset_y);

            AirfieldCanvas().Children().Append(
                button);
        }
    }
'@

$cppText =
    $cppText.Substring(0, $start) +
    $replacement +
    $cppText.Substring($closeBrace + 1)

Set-Content `
    -Path $viewCpp `
    -Value $cppText `
    -Encoding utf8

Write-Host ""
Write-Host "render_nodes() repaired."
Write-Host ""
Write-Host "Removed:"
Write-Host "  duplicate legacy intermediate Border loop"
Write-Host ""
Write-Host "Restored:"
Write-Host "  F / H / J / 18L controller buttons from airfield.nodes()"
Write-Host "  52x34 controller hit targets"
Write-Host "  controller buttons appended last so they render on top"
Write-Host "  visual-only offsets away from F1 / H1 / J1"
Write-Host ""
Write-Host "Route/node domain coordinates are unchanged."
Write-Host "Expected regression baseline remains 95/95."
