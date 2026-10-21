#pragma once

#include "i_local_inference.h"
#include "llama_cpp_inference.h"

#include <memory>
#include <mutex>
#include <utility>

namespace xsim::inference
{
    // Keeps WinUI startup fast. The GGUF is loaded on the first inference
    // request and then reused for subsequent requests.
    class LazyLlamaInference final
        : public ILocalInference
    {
    public:
        explicit LazyLlamaInference(
            LlamaModelOptions options)
            : options_(std::move(options))
        {
        }

        [[nodiscard]]
        InferenceResult infer(
            const InferenceRequest& request) override
        {
            std::scoped_lock lock(mutex_);

            if (!inner_)
            {
                inner_ =
                    std::make_unique<LlamaCppInference>(
                        options_);
            }

            return inner_->infer(request);
        }

    private:
        LlamaModelOptions options_;
        std::unique_ptr<LlamaCppInference> inner_;
        std::mutex mutex_;
    };
}
