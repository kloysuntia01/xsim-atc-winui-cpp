#include <gtest/gtest.h>

#include "llama_model.h"

#include <filesystem>
#include <stdexcept>

namespace
{
    TEST(LlamaModelTests, EmptyModelPathIsRejected)
    {
        EXPECT_THROW(
            static_cast<void>(xsim::inference::LlamaModel({})),
            std::invalid_argument);
    }

    TEST(LlamaModelTests, MissingModelFileIsRejected)
    {
        const auto missing =
            std::filesystem::temp_directory_path() /
            "xsimatc-model-that-does-not-exist.gguf";

        std::error_code error;
        std::filesystem::remove(missing, error);

        EXPECT_THROW(
            static_cast<void>(xsim::inference::LlamaModel({ .model_path = missing })),
            std::runtime_error);
    }
}
