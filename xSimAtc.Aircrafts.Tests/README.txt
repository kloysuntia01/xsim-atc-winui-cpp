xSimAtc Transponder Tests

Drop transponder_tests.cpp into:
C:\Dev\xsimatc-cpp\xSimAtc.Aircrafts.Tests

Assumptions:
- Google Test project already exists.
- xSimAtc.Aircrafts.Tests already references xSimAtc.Aircrafts.
- The test project can include transponder.h.
- The test project still uses pch.h.

Tests:
- Start emits at least one signal.
- Signal contains the correct callsign.
- Signal contains the correct aircraft GUID.
- Multiple signals are emitted.
- stop() prevents future emissions after the jthread joins.
