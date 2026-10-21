#include "pch.h"
#include "llama_model.h"

#include <llama.h>

namespace
{
    class LlamaBackendRuntime final
    {
    public:
        LlamaBackendRuntime()
        {
            llama_backend_init();
        }

        ~LlamaBackendRuntime()
        {
            llama_backend_free();
        }

        LlamaBackendRuntime(const LlamaBackendRuntime&) = delete;
        LlamaBackendRuntime& operator=(const LlamaBackendRuntime&) = delete;
    };

    LlamaBackendRuntime& llama_backend_runtime()
    {
        static LlamaBackendRuntime runtime;
        return runtime;
    }
}

namespace xsim::inference
{
    struct LlamaModel::Impl final
    {
        explicit Impl(llama_model* model) noexcept
            : model(model)
        {
        }

        ~Impl()
        {
            llama_model_free(model);
        }

        llama_model* model{};
    };

    LlamaModel::LlamaModel(LlamaModelOptions options)
        : options_(std::move(options))
    {
        if (options_.model_path.empty())
        {
            throw std::invalid_argument("Llama model path is required.");
        }

        if (!std::filesystem::is_regular_file(options_.model_path))
        {
            throw std::runtime_error(
                "Llama model file does not exist: " +
                options_.model_path.string());
        }

        // Initialize the process-wide llama/ggml backend before loading a model.
        static_cast<void>(llama_backend_runtime());

        auto params = llama_model_default_params();
        params.n_gpu_layers = options_.gpu_layers;
        params.load_mode =
            options_.use_mmap
                ? LLAMA_LOAD_MODE_MMAP
                : LLAMA_LOAD_MODE_NONE;

        const auto model_path = options_.model_path.string();
        auto* model = llama_model_load_from_file(
            model_path.c_str(),
            params);

        if (model == nullptr)
        {
            throw std::runtime_error(
                "Unable to load Llama GGUF model: " + model_path);
        }

        impl_ = std::make_unique<Impl>(model);
    }

    LlamaModel::~LlamaModel() = default;

    LlamaModel::LlamaModel(LlamaModel&&) noexcept = default;

    LlamaModel& LlamaModel::operator=(LlamaModel&&) noexcept = default;

    bool LlamaModel::loaded() const noexcept
    {
        return impl_ != nullptr && impl_->model != nullptr;
    }

    const LlamaModelOptions& LlamaModel::options() const noexcept
    {
        return options_;
    }

    void* LlamaModel::native_handle() const noexcept
    {
        return impl_ == nullptr
            ? nullptr
            : static_cast<void*>(impl_->model);
    }
}
