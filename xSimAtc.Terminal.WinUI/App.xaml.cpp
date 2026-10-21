#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "MainViewModel.h"
#include "TowerViewModel.h"

#include "../xSimAtc.Advisory/llama_atc_advisory_service.h"
#include "../xSimAtc.Inference/lazy_llama_inference.h"
#include "../xSimAtc.Speech/windows_text_to_speech.h"

namespace winrt::xSimAtc_Terminal_WinUI::implementation
{
    App::App()
        : message_queue_(
            std::make_shared<xsim::communications::MessageQueue>()),
          aircraft_mediator_(
            std::make_shared<xsim::communications::AircraftMediator>(
                message_queue_))
    {
        struct Seed final
        {
            const char* call_sign;
            double x;
            double y;
            double velocity_x;
            double velocity_y;
        };

        constexpr std::array<Seed, 5> seeds{{
            { "N123",   35.0, 250.0,  17.0, -11.0 },
            { "AAL245", 250.0, 35.0, -13.0,  16.0 },
            { "FDX606", 465.0, 250.0, -18.0,  10.0 },
            { "DAL777", 250.0, 465.0,  12.0, -17.0 },
            { "UAL808", 75.0,  75.0,  15.0,  14.0 }
        }};

        for (std::size_t index = 0; index < seeds.size(); ++index)
        {
            const auto& seed = seeds[index];

            aircraft_[index] =
                std::make_unique<xsim::aircrafts::Aircraft>(
                    seed.call_sign,
                    aircraft_mediator_,
                    seed.x,
                    seed.y,
                    seed.velocity_x,
                    seed.velocity_y);

            aircraft_[index]->start_transponder();
        }
    }

    void App::OnLaunched(
        Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        auto main_view_model = winrt::make<MainViewModel>();

        // First real Tower -> local LLM advisory composition.
        // LazyLlamaInference defers the ~model-load cost until the first
        // Request Advisory click, then reuses the loaded model.
        auto local_inference =
            std::make_shared<
                xsim::inference::LazyLlamaInference>(
                xsim::inference::LlamaModelOptions{
                    .model_path =
                        R"(C:\AI\Models\Qwen3-0.6B-Q4_K_M.gguf)"
                });

        auto advisory_service =
            std::make_shared<
                xsim::advisory::LlamaAtcAdvisoryService>(
                    local_inference);

        auto text_to_speech =
            std::make_shared<
                xsimatc::speech::WindowsTextToSpeech>();

        auto tower_view_model =
            winrt::make<TowerViewModel>(
                message_queue_,
                advisory_service,
                text_to_speech);

        window_ = winrt::make<MainWindow>(
            main_view_model,
            tower_view_model,
            message_queue_);

        window_.Activate();
    }
}
