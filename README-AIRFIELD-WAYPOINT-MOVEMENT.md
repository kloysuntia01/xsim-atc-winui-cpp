# Airfield Waypoint Movement Slice

Adds center waypoints plus a movement cursor that interpolates between waypoints.

This is deliberately domain-only: no WinUI timer or animation yet. That keeps the movement math testable before wiring UI ticks.
