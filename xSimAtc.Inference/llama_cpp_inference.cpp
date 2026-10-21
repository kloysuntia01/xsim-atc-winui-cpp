#include "pch.h"
#include "llama_cpp_inference.h"
#include "inference_text_normalizer.h"

#include <llama.h>

#include <algorithm>
#include <array>
#include <limits>
#include <vector>

namespace
{
    class LlamaContext final
    {
    public:
        explicit LlamaContext(llama_context* context) noexcept
            : context_(context)
        {
        }

        ~LlamaContext()
        {
            llama_free(context_);
        }

        LlamaContext(const LlamaContext&) = delete;
        LlamaContext& operator=(const LlamaContext&) = delete;

        [[nodiscard]]
        llama_context* get() const noexcept
        {
            return context_;
        }

    private:
        llama_context* context_{};
    };

    class LlamaSampler final
    {
    public:
        explicit LlamaSampler(llama_sampler* sampler) noexcept
            : sampler_(sampler)
        {
        }

        ~LlamaSampler()
        {
            llama_sampler_free(sampler_);
        }

        LlamaSampler(const LlamaSampler&) = delete;
        LlamaSampler& operator=(const LlamaSampler&) = delete;

        [[nodiscard]]
        llama_sampler* get() const noexcept
        {
            return sampler_;
        }

    private:
        llama_sampler* sampler_{};
    };

    class LlamaBatch final
    {
    public:
        explicit LlamaBatch(llama_batch_ext* batch) noexcept
            : batch_(batch)
        {
        }

        ~LlamaBatch()
        {
            llama_batch_ext_free(batch_);
        }

        LlamaBatch(const LlamaBatch&) = delete;
        LlamaBatch& operator=(const LlamaBatch&) = delete;

        [[nodiscard]]
        llama_batch_ext* get() const noexcept
        {
            return batch_;
        }

    private:
        llama_batch_ext* batch_{};
    };

    void set_batch_tokens(
        llama_batch_ext* batch,
        const llama_token* tokens,
        const std::int32_t token_count,
        const llama_pos first_position)
    {
        llama_batch_ext_clear(batch);

        for (std::int32_t i = 0; i < token_count; ++i)
        {
            const auto index =
                llama_batch_ext_add_token(batch, 0, tokens[i]);

            const llama_pos position = first_position + i;
            llama_batch_ext_set_pos(batch, index, &position);
        }

        llama_batch_ext_set_output_logits(
            batch,
            token_count - 1,
            true);
    }

    std::string token_piece(
        const llama_vocab* vocab,
        const llama_token token)
    {
        std::array<char, 256> buffer{};

        const auto length = llama_token_to_piece(
            vocab,
            token,
            buffer.data(),
            static_cast<std::int32_t>(buffer.size()),
            0,
            true);

        if (length < 0)
        {
            throw std::runtime_error(
                "llama_token_to_piece failed while generating text.");
        }

        return std::string(
            buffer.data(),
            static_cast<std::size_t>(length));
    }

    std::string format_prompt(
        const llama_model* model,
        const xsim::inference::InferenceRequest& request)
    {
        if (!request.generation.use_chat_template)
        {
            return request.prompt;
        }

        const auto* chat_template =
            llama_model_chat_template(model, nullptr);

        if (chat_template == nullptr)
        {
            throw std::runtime_error(
                "Chat-template mode was requested but the model has no default chat template.");
        }

        std::vector<llama_chat_message> messages;
        messages.reserve(
            request.generation.system_prompt.empty() ? 1U : 2U);

        if (!request.generation.system_prompt.empty())
        {
            messages.push_back(
                {
                    "system",
                    request.generation.system_prompt.c_str()
                });
        }

        messages.push_back(
            {
                "user",
                request.prompt.c_str()
            });

        const auto required_length =
            llama_chat_apply_template(
                chat_template,
                messages.data(),
                messages.size(),
                true,
                nullptr,
                0);

        if (required_length < 0)
        {
            throw std::runtime_error(
                "Unable to apply the model chat template.");
        }

        std::vector<char> formatted(
            static_cast<std::size_t>(required_length) + 1U);

        const auto formatted_length =
            llama_chat_apply_template(
                chat_template,
                messages.data(),
                messages.size(),
                true,
                formatted.data(),
                static_cast<std::int32_t>(formatted.size()));

        if (formatted_length < 0)
        {
            throw std::runtime_error(
                "Unable to format the inference chat prompt.");
        }

        return std::string(
            formatted.data(),
            static_cast<std::size_t>(formatted_length));
    }

    void configure_sampler(
        llama_sampler* sampler,
        const xsim::inference::GenerationOptions& options)
    {
        if (options.temperature <= 0.0F)
        {
            llama_sampler_chain_add(
                sampler,
                llama_sampler_init_greedy());

            return;
        }

        if (options.top_k > 0)
        {
            llama_sampler_chain_add(
                sampler,
                llama_sampler_init_top_k(options.top_k));
        }

        if (options.top_p < 1.0F)
        {
            llama_sampler_chain_add(
                sampler,
                llama_sampler_init_top_p(
                    options.top_p,
                    1));
        }

        llama_sampler_chain_add(
            sampler,
            llama_sampler_init_temp(options.temperature));

        llama_sampler_chain_add(
            sampler,
            llama_sampler_init_dist(options.seed));
    }
}

namespace xsim::inference
{
    void validate_inference_request(
        const InferenceRequest& request)
    {
        if (request.prompt.empty())
        {
            throw std::invalid_argument(
                "Inference prompt is required.");
        }

        if (request.generation.max_tokens <= 0)
        {
            throw std::invalid_argument(
                "Inference max_tokens must be greater than zero.");
        }

        if (request.generation.context_tokens == 0)
        {
            throw std::invalid_argument(
                "Inference context_tokens must be greater than zero.");
        }

        if (request.generation.temperature < 0.0F)
        {
            throw std::invalid_argument(
                "Inference temperature cannot be negative.");
        }

        if (request.generation.top_k < 0)
        {
            throw std::invalid_argument(
                "Inference top_k cannot be negative.");
        }

        if (request.generation.top_p <= 0.0F ||
            request.generation.top_p > 1.0F)
        {
            throw std::invalid_argument(
                "Inference top_p must be greater than zero and no greater than one.");
        }
    }

    LlamaCppInference::LlamaCppInference(
        LlamaModelOptions options)
        : model_(std::move(options))
    {
    }

    InferenceResult LlamaCppInference::infer(
        const InferenceRequest& request)
    {
        validate_inference_request(request);

        auto* model = static_cast<llama_model*>(model_.native_handle());

        if (model == nullptr)
        {
            throw std::runtime_error(
                "Llama model is not loaded.");
        }

        const auto* vocab = llama_model_get_vocab(model);

        if (vocab == nullptr)
        {
            throw std::runtime_error(
                "Llama model did not provide a vocabulary.");
        }

        const auto formatted_prompt =
            format_prompt(model, request);

        if (formatted_prompt.size() >
            static_cast<std::size_t>(
                std::numeric_limits<std::int32_t>::max()))
        {
            throw std::invalid_argument(
                "Inference prompt exceeds llama.cpp text length limits.");
        }

        const auto prompt_length =
            static_cast<std::int32_t>(formatted_prompt.size());

        const auto required_prompt_tokens = llama_tokenize(
            vocab,
            formatted_prompt.c_str(),
            prompt_length,
            nullptr,
            0,
            true,
            true);

        if (required_prompt_tokens >= 0)
        {
            throw std::runtime_error(
                "Unable to determine Llama prompt token count.");
        }

        const auto prompt_token_count =
            -required_prompt_tokens;

        std::vector<llama_token> prompt_tokens(
            static_cast<std::size_t>(prompt_token_count));

        const auto tokenized_count = llama_tokenize(
            vocab,
            formatted_prompt.c_str(),
            prompt_length,
            prompt_tokens.data(),
            static_cast<std::int32_t>(prompt_tokens.size()),
            true,
            true);

        if (tokenized_count < 0)
        {
            throw std::runtime_error(
                "Unable to tokenize Llama inference prompt.");
        }

        const auto required_context =
            static_cast<std::uint64_t>(prompt_token_count) +
            static_cast<std::uint64_t>(
                request.generation.max_tokens);

        if (required_context >
            request.generation.context_tokens)
        {
            throw std::invalid_argument(
                "Inference context_tokens is too small for the prompt and max_tokens.");
        }

        if (required_context >
            static_cast<std::uint64_t>(
                std::numeric_limits<std::uint32_t>::max()))
        {
            throw std::invalid_argument(
                "Inference context size exceeds llama.cpp limits.");
        }

        auto context_params =
            llama_context_default_params();

        context_params.n_ctx =
            request.generation.context_tokens;

        context_params.n_batch =
            request.generation.context_tokens;

        context_params.no_perf = true;

        LlamaContext context(
            llama_init_from_model(
                model,
                context_params));

        if (context.get() == nullptr)
        {
            throw std::runtime_error(
                "Unable to create Llama inference context.");
        }

        auto sampler_params =
            llama_sampler_chain_default_params();

        sampler_params.no_perf = true;

        LlamaSampler sampler(
            llama_sampler_chain_init(sampler_params));

        if (sampler.get() == nullptr)
        {
            throw std::runtime_error(
                "Unable to create Llama sampler.");
        }

        configure_sampler(
            sampler.get(),
            request.generation);

        LlamaBatch batch(
            llama_batch_ext_init(context.get()));

        if (batch.get() == nullptr)
        {
            throw std::runtime_error(
                "Unable to create Llama inference batch.");
        }

        auto active_tokens = prompt_tokens.data();
        auto active_token_count = prompt_token_count;

        set_batch_tokens(
            batch.get(),
            active_tokens,
            active_token_count,
            0);

        if (llama_model_has_encoder(model))
        {
            if (llama_process(
                    context.get(),
                    LLAMA_PROCESS_TYPE_ENCODE,
                    batch.get()) != 0)
            {
                throw std::runtime_error(
                    "Llama encoder processing failed.");
            }

            auto decoder_start =
                llama_model_decoder_start_token(model);

            if (decoder_start == LLAMA_TOKEN_NULL)
            {
                decoder_start = llama_vocab_bos(vocab);
            }

            active_tokens = &decoder_start;
            active_token_count = 1;

            set_batch_tokens(
                batch.get(),
                active_tokens,
                active_token_count,
                0);
        }

        InferenceResult result;
        result.prompt_tokens = prompt_token_count;

        llama_token sampled_token = LLAMA_TOKEN_NULL;

        for (
            std::int32_t position = 0;
            result.generated_tokens <
                request.generation.max_tokens;)
        {
            if (llama_process(
                    context.get(),
                    LLAMA_PROCESS_TYPE_DECODE,
                    batch.get()) != 0)
            {
                throw std::runtime_error(
                    "Llama decode failed.");
            }

            position += active_token_count;

            sampled_token = llama_sampler_sample(
                sampler.get(),
                context.get(),
                -1);

            if (llama_vocab_is_eog(
                    vocab,
                    sampled_token))
            {
                result.stopped_on_eog = true;
                result.finish_reason =
                    InferenceFinishReason::EndOfGeneration;
                break;
            }

            result.raw_text +=
                token_piece(vocab, sampled_token);

            ++result.generated_tokens;

            active_tokens = &sampled_token;
            active_token_count = 1;

            set_batch_tokens(
                batch.get(),
                active_tokens,
                active_token_count,
                position);
        }

        result.final_text =
            normalize_inference_text(result.raw_text);

        return result;
    }

    const LlamaModel& LlamaCppInference::model() const noexcept
    {
        return model_;
    }
}
