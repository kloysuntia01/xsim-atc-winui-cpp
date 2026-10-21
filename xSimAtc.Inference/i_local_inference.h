#pragma once

#include "inference_request.h"
#include "inference_result.h"

namespace xsim::inference
{
    class ILocalInference
    {
    public:
        virtual ~ILocalInference() = default;

        [[nodiscard]]
        virtual InferenceResult infer(
            const InferenceRequest& request) = 0;
    };
}
