# xSimAtc Airfield Interactive Route Slice

This slice intentionally stops before aircraft animation.

## Added

- `AirfieldRouteSelection` domain helper.
- Four TDD tests:
  - Starts empty.
  - Adds `F -> H -> J -> 18L` in click order.
  - Ignores duplicate nodes.
  - Clears the route.
- Clickable aircraft control for `FDX606`.
- Clickable circular airfield-node buttons.
- Selected-node visual state.
- Highlighted route segments.
- Route summary card.
- Aircraft marker is offset from its actual position so node `F` remains visible and clickable.

## Interaction

1. Open **Airfield**.
2. Click **FDX606**.
3. Click **F**, **H**, **J**, **18L**.
4. Confirm the route summary reads:

   `F -> H -> J -> 18L`

5. Confirm the selected nodes are highlighted and connected.

No movement or Rx animation is introduced here.
