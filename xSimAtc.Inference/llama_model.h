#pragma once

#include "llama_model_options.h"

#include <memory>

namespace xsim::inference
{
    class LlamaCppInference;

    class LlamaModel final
    {
    public:
        explicit LlamaModel(LlamaModelOptions options);
        ~LlamaModel();

        LlamaModel(const LlamaModel&) = delete;
        LlamaModel& operator=(const LlamaModel&) = delete;

        LlamaModel(LlamaModel&&) noexcept;
        LlamaModel& operator=(LlamaModel&&) noexcept;

        [[nodiscard]]
        bool loaded() const noexcept;

        [[nodiscard]]
        const LlamaModelOptions& options() const noexcept;

    private:
        friend class LlamaCppInference;

        struct Impl;

        [[nodiscard]]
        void* native_handle() const noexcept;

        LlamaModelOptions options_;
        std::unique_ptr<Impl> impl_;
    };
}
