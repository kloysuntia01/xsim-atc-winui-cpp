#include "windows_text_to_speech.h"

#include <exception>
#include <iostream>
#include <string>

namespace
{
    [[nodiscard]]
    std::string phrase_from_args(
        const int argc,
        char* argv[])
    {
        if (argc <= 1)
        {
            return "FedEx six zero six, cleared to land.";
        }

        std::string phrase;

        for (int index = 1; index < argc; ++index)
        {
            if (!phrase.empty())
            {
                phrase.push_back(' ');
            }

            phrase += argv[index];
        }

        return phrase;
    }
}

int main(
    const int argc,
    char* argv[])
{
    try
    {
        const std::string phrase =
            phrase_from_args(argc, argv);

        std::cout
            << "xSimAtc local TTS smoke\n"
            << "Speaking: "
            << phrase
            << '\n';

        xsimatc::speech::WindowsTextToSpeech tts;
        tts.speak(phrase);

        std::cout << "Speech completed.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "TTS smoke failed: "
            << exception.what()
            << '\n';

        return 1;
    }
}
