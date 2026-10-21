#pragma once

#include "i_atc_advisory_service.h"

#include "../xSimAtc.Inference/i_local_inference.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace xsim::advisory
{
    class LlamaAtcAdvisoryService final
        : public IAtcAdvisoryService
    {
    public:
        explicit LlamaAtcAdvisoryService(
            std::shared_ptr<xsim::inference::ILocalInference>
                inference)
            : inference_(std::move(inference))
        {
            if (!inference_)
            {
                throw std::invalid_argument(
                    "LlamaAtcAdvisoryService requires local inference.");
            }
        }

        [[nodiscard]]
        AtcAdvisoryResult advise(
            const AtcAdvisoryRequest& request) const override
        {
            std::ostringstream prompt;

            prompt
                << "Runway " << request.runway
                << " status: " << request.runway_status << ".\n"
                << "Aircraft " << request.call_sign
                << " status: " << request.aircraft_status << ".\n"
                << "Give one short ATC safety advisory. "
                << "Do not change simulation state. "
                << "Do not explain your reasoning. /no_think";

            xsim::inference::InferenceRequest inference_request;
            inference_request.prompt = prompt.str();
            inference_request.generation.max_tokens = 48;
            inference_request.generation.context_tokens = 2048;
            inference_request.generation.temperature = 0.0F;
            inference_request.generation.use_chat_template = true;
            inference_request.generation.system_prompt =
                "You are a concise air traffic control advisory assistant. "
                "Give advisory text only. "
                "The simulation World is authoritative and you must never "
                "claim to change aircraft, runway, or World state.";

            const auto inference_result =
                inference_->infer(inference_request);

            AtcAdvisoryResult result;
            result.text =
                inference_result.final_text.empty()
                    ? inference_result.raw_text
                    : inference_result.final_text;

            if (result.text.empty())
            {
                result.text =
                    "Maintain current state and request controller review.";
            }

            // This service has no World mutation dependency by design.
            result.world_state_changed = false;
            return result;
        }

    private:
        std::shared_ptr<xsim::inference::ILocalInference>
            inference_;
    };
}
