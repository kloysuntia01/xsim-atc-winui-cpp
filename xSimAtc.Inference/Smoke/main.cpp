#include "llama_cpp_inference.h"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr
            << "Usage: xSimAtc.Inference.Smoke <model.gguf> [prompt]\n";
        return 2;
    }

    const std::string prompt =
        argc == 3
            ? argv[2]
            : "Reply with exactly: XSIM READY /no_think";

    try
    {
        xsim::inference::LlamaCppInference inference(
            { .model_path = argv[1] });

        xsim::inference::InferenceRequest request;
        request.prompt = prompt;
        request.generation.max_tokens = 32;
        request.generation.context_tokens = 2048;
        request.generation.temperature = 0.0F;
        request.generation.use_chat_template = true;
        request.generation.system_prompt =
            "You are a concise xSimAtc simulator assistant. "
            "Follow the user's requested output format exactly.";

        const auto result =
            inference.infer(request);

        std::cout
            << "XSIM INFERENCE READY\n"
            << "Prompt tokens: "
            << result.prompt_tokens << '\n'
            << "Generated tokens: "
            << result.generated_tokens << '\n'
            << "Finish reason: "
            << xsim::inference::inference_finish_reason_name(
                result.finish_reason)
            << "\nRaw:\n"
            << result.raw_text
            << "\nFinal:\n"
            << result.final_text
            << '\n';

        return result.final_text.empty() ? 1 : 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
