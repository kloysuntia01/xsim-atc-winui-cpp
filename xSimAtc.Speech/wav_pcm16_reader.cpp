#include "pch.h"
#include "wav_pcm16_reader.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::uint16_t read_u16(std::istream& input)
    {
        std::array<unsigned char, 2> bytes{};
        input.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

        if (!input)
        {
            throw std::runtime_error("Unexpected end of WAV file.");
        }

        return static_cast<std::uint16_t>(bytes[0]) |
            (static_cast<std::uint16_t>(bytes[1]) << 8);
    }

    std::uint32_t read_u32(std::istream& input)
    {
        std::array<unsigned char, 4> bytes{};
        input.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

        if (!input)
        {
            throw std::runtime_error("Unexpected end of WAV file.");
        }

        return static_cast<std::uint32_t>(bytes[0]) |
            (static_cast<std::uint32_t>(bytes[1]) << 8) |
            (static_cast<std::uint32_t>(bytes[2]) << 16) |
            (static_cast<std::uint32_t>(bytes[3]) << 24);
    }

    std::string read_fourcc(std::istream& input)
    {
        std::array<char, 4> value{};
        input.read(value.data(), value.size());

        if (!input)
        {
            throw std::runtime_error("Unexpected end of WAV file.");
        }

        return { value.data(), value.size() };
    }
}

namespace xsim::speech
{
    WavPcm16Audio WavPcm16Reader::read_mono_16khz(
        const std::filesystem::path& audio_file)
    {
        std::ifstream input(audio_file, std::ios::binary);

        if (!input)
        {
            throw std::runtime_error(
                "Unable to open WAV file: " + audio_file.string());
        }

        if (read_fourcc(input) != "RIFF")
        {
            throw std::runtime_error("Audio file is not a RIFF WAV file.");
        }

        (void)read_u32(input);

        if (read_fourcc(input) != "WAVE")
        {
            throw std::runtime_error("Audio file is not a WAVE file.");
        }

        std::uint16_t audio_format{};
        std::uint16_t channel_count{};
        std::uint32_t sample_rate{};
        std::uint16_t bits_per_sample{};
        std::vector<unsigned char> pcm_bytes;
        bool found_format = false;
        bool found_data = false;

        while (input && !(found_format && found_data))
        {
            const auto chunk_id = read_fourcc(input);
            const auto chunk_size = read_u32(input);

            if (chunk_id == "fmt ")
            {
                if (chunk_size < 16)
                {
                    throw std::runtime_error("WAV fmt chunk is invalid.");
                }

                audio_format = read_u16(input);
                channel_count = read_u16(input);
                sample_rate = read_u32(input);
                (void)read_u32(input);
                (void)read_u16(input);
                bits_per_sample = read_u16(input);

                if (chunk_size > 16)
                {
                    input.seekg(static_cast<std::streamoff>(chunk_size - 16), std::ios::cur);
                }

                found_format = true;
            }
            else if (chunk_id == "data")
            {
                pcm_bytes.resize(chunk_size);
                input.read(
                    reinterpret_cast<char*>(pcm_bytes.data()),
                    static_cast<std::streamsize>(pcm_bytes.size()));

                if (!input)
                {
                    throw std::runtime_error("WAV data chunk is truncated.");
                }

                found_data = true;
            }
            else
            {
                input.seekg(static_cast<std::streamoff>(chunk_size), std::ios::cur);
            }

            if ((chunk_size & 1U) != 0U)
            {
                input.seekg(1, std::ios::cur);
            }
        }

        if (!found_format || !found_data)
        {
            throw std::runtime_error("WAV file is missing fmt or data chunk.");
        }

        if (audio_format != 1 || channel_count != 1 ||
            sample_rate != 16000 || bits_per_sample != 16)
        {
            throw std::runtime_error(
                "Whisper input must be PCM16 mono 16 kHz WAV audio.");
        }

        if ((pcm_bytes.size() % 2U) != 0U)
        {
            throw std::runtime_error("PCM16 WAV data has an invalid byte count.");
        }

        WavPcm16Audio audio;
        audio.sample_rate = static_cast<int>(sample_rate);
        audio.samples.reserve(pcm_bytes.size() / 2U);

        for (std::size_t i = 0; i < pcm_bytes.size(); i += 2U)
        {
            const auto raw = static_cast<std::uint16_t>(pcm_bytes[i]) |
                (static_cast<std::uint16_t>(pcm_bytes[i + 1]) << 8);
            const auto sample = static_cast<std::int16_t>(raw);
            audio.samples.push_back(static_cast<float>(sample) / 32768.0F);
        }

        return audio;
    }
}
