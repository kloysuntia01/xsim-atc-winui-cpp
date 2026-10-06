#include "pch.h"

#include "whisper_model.h"
#include "whisper_model_options.h"

#include <filesystem>
#include <stdexcept>

namespace
{
    TEST(WhisperModelOptionsTests, DefaultLanguageIsEnglish)
    {
        const xsim::speech::WhisperModelOptions options;

        EXPECT_EQ(options.language, "en");
    }

    TEST(WhisperModelOptionsTests, DefaultThreadCountIsFour)
    {
        const xsim::speech::WhisperModelOptions options;

        EXPECT_EQ(options.thread_count, 4);
    }

    TEST(WhisperModelTests, InvalidModelPathThrows)
    {
        xsim::speech::WhisperModelOptions options;
        options.model_path = std::filesystem::path{
            "this-model-does-not-exist.ggml.bin"
        };

        EXPECT_THROW(
            xsim::speech::WhisperModel model{ options },
            std::runtime_error);
    }
}
