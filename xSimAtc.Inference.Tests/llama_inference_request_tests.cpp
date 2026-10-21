#include <gtest/gtest.h>

#include "inference_text_normalizer.h"
#include "llama_cpp_inference.h"

#include <stdexcept>
#include <string>

namespace
{
    TEST(LlamaInferenceRequestTests, EmptyPromptIsRejected)
    {
        EXPECT_THROW(
            xsim::inference::validate_inference_request({}),
            std::invalid_argument);
    }

    TEST(LlamaInferenceRequestTests, NonPositiveMaxTokensIsRejected)
    {
        xsim::inference::InferenceRequest request;
        request.prompt = "XSIM READY";
        request.generation.max_tokens = 0;

        EXPECT_THROW(
            xsim::inference::validate_inference_request(request),
            std::invalid_argument);
    }

    TEST(LlamaInferenceRequestTests, ZeroContextTokensIsRejected)
    {
        xsim::inference::InferenceRequest request;
        request.prompt = "XSIM READY";
        request.generation.context_tokens = 0;

        EXPECT_THROW(
            xsim::inference::validate_inference_request(request),
            std::invalid_argument);
    }

    TEST(LlamaInferenceRequestTests, NegativeTemperatureIsRejected)
    {
        xsim::inference::InferenceRequest request;
        request.prompt = "XSIM READY";
        request.generation.temperature = -0.1F;

        EXPECT_THROW(
            xsim::inference::validate_inference_request(request),
            std::invalid_argument);
    }

    TEST(LlamaInferenceRequestTests, NegativeTopKIsRejected)
    {
        xsim::inference::InferenceRequest request;
        request.prompt = "XSIM READY";
        request.generation.top_k = -1;

        EXPECT_THROW(
            xsim::inference::validate_inference_request(request),
            std::invalid_argument);
    }

    TEST(LlamaInferenceRequestTests, TopPOutsideRangeIsRejected)
    {
        xsim::inference::InferenceRequest request;
        request.prompt = "XSIM READY";
        request.generation.top_p = 1.1F;

        EXPECT_THROW(
            xsim::inference::validate_inference_request(request),
            std::invalid_argument);
    }

    TEST(InferenceTextNormalizerTests, LeadingThinkBlockIsRemoved)
    {
        const std::string raw =
            "<think>\n\n</think>\n\nXSIM READY";

        EXPECT_EQ(
            xsim::inference::normalize_inference_text(raw),
            "XSIM READY");
    }

    TEST(InferenceTextNormalizerTests, LeadingWhitespaceBeforeThinkBlockIsAllowed)
    {
        const std::string raw =
            "\n  <think>internal reasoning</think>\nResult";

        EXPECT_EQ(
            xsim::inference::normalize_inference_text(raw),
            "Result");
    }

    TEST(InferenceTextNormalizerTests, OrdinaryTextIsPreserved)
    {
        const std::string raw =
            "XSIM READY";

        EXPECT_EQ(
            xsim::inference::normalize_inference_text(raw),
            raw);
    }

    TEST(InferenceTextNormalizerTests, UnterminatedThinkBlockIsPreserved)
    {
        const std::string raw =
            "<think>partial reasoning";

        EXPECT_EQ(
            xsim::inference::normalize_inference_text(raw),
            raw);
    }
}
