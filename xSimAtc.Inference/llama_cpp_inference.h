#pragma once

#include "i_local_inference.h"
#include "llama_model.h"

namespace xsim::inference
{
    void validate_inference_request(
        const InferenceRequest& request);

    class LlamaCppInference final
        : public ILocalInference
    {
    public:
        explicit LlamaCppInference(
            LlamaModelOptions options);

        [[nodiscard]]
        InferenceResult infer(
            const InferenceRequest& request) override;

        [[nodiscard]]
        const LlamaModel& model() const noexcept;

    private:
        LlamaModel model_;
    };
}
