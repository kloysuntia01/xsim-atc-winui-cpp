# xSimAtc.Inference.Cli

Interactive local llama.cpp/Qwen console for exercising `xSimAtc.Inference`
without launching the WinUI application.

## Build

```powershell
cmake -S . -B .\build -G "Visual Studio 18 2026" -A x64

cmake --build .\build `
    --config Debug `
    --target xSimAtc.Inference.Cli `
    --parallel 1
```

## Run

Default model:

```powershell
.\build\xSimAtc.Inference.Cli\Debug\xSimAtc.Inference.Cli.exe
```

Choose another model:

```powershell
.\build\xSimAtc.Inference.Cli\Debug\xSimAtc.Inference.Cli.exe `
    --model "C:\AI\Models\Qwen3-1.7B-Q4_K_M.gguf"
```

Start without speech:

```powershell
.\build\xSimAtc.Inference.Cli\Debug\xSimAtc.Inference.Cli.exe `
    --no-tts
```

## Commands

```text
/help
/show
/tts on
/tts off
/tokens 256
/system You are a concise ATC assistant.
/exit
```

Anything else is sent directly to the local model using the model-native chat
template. `InferenceResult.final_text` is printed and, when TTS is enabled,
spoken through the existing Windows SAPI implementation.
