# xSimAtc — Option-B render_nodes controller repair

Starting verified checkpoint: **95/95**.

The actual `render_nodes()` source revealed the root cause:

1. It rendered `intermediate_nodes()` as interactive buttons.
2. It then rendered the same `intermediate_nodes()` again as legacy non-interactive
   `Border` panels.
3. The original `airfield.nodes()` loop for the controller nodes
   `F / H / J / 18L` was gone entirely.

That explains why F/H/J/18L could not be found and why the previous visibility
patch could not find the old 36x28 controller block.

This repair replaces only `render_nodes()` with a clean two-layer implementation:

```text
physical/intermediate nodes first
-> 30x22 interactive buttons

controller nodes second
-> F / H / J / 18L
-> 52x34 interactive buttons
-> appended last so they render above the physical nodes
-> visual-only offsets to separate them from F1/H1/J1
```

No domain coordinates or graph routing are changed.

Expected regression baseline remains **95/95**.
