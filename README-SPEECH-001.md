# SPEECH-001 — Deterministic ATC speech formatting

Purpose:

- Keep model/display text unchanged.
- Convert callsigns and runway identifiers only on the TTS path.
- No LLM call.
- No World mutation.
- No ATC decision logic.

Examples:

- `FDX606` -> `Foxtrot Delta X-ray Six Zero Six`
- `ABC123` -> `Alpha Bravo Charlie One Two Three`
- `Runway 18R` -> `Runway One Eight Right`
- `Runway 4L` -> `Runway Four Left`

CLI flow:

```text
InferenceResult.final_text
    ├── console display (unchanged)
    └── AtcSpeechFormatter::format(...)
            └── WindowsTextToSpeech
```

## Apply

From repo root:

```powershell
.\apply-speech001-wiring.ps1
```

Then configure/build:

```powershell
cmake -S . -B .\build -G "Visual Studio 18 2026" -A x64
cmake --build .\build --config Debug --target xSimAtc.Inference.Cli --parallel 1
```

The test file is provided as a focused test source. Wire it into the existing
`xSimAtc.Speech.Tests` target according to the repo's current test layout.
