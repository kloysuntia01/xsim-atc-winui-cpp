# xSimAtc — Option B Vector Airfield Slice

This slice replaces the simple programmatic runway background with a packaged SVG
airfield asset while keeping all operational elements live.

## Static vector SVG

- runways
- runway labels
- taxiway pavement / centerlines
- apron
- terminal
- gates
- parked aircraft silhouettes

## Dynamic WinUI overlay

- selected taxi route
- clickable taxi nodes
- aircraft marker
- tower marker

The selected taxi route is rendered as one WinUI `Path` whose `Data` is built with
SVG-style `M` / `L` path commands from the selected route positions.

Example:

`M Fx,Fy L Hx,Hy L Jx,Jy L 18Lx,18Ly`

This is intentionally a visual slice. The domain test baseline stays at 60/60.
