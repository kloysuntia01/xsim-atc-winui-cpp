#include "atc_speech_formatter.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
    void expect_equal(
        const std::string& actual,
        const std::string& expected,
        const char* name)
    {
        if (actual != expected)
        {
            std::cerr
                << "FAIL: " << name << '\n'
                << "expected: " << expected << '\n'
                << "actual:   " << actual << '\n';

            std::exit(1);
        }

        std::cout << "PASS: " << name << '\n';
    }
}

int main()
{
    const xsimatc::speech::AtcSpeechFormatter formatter;

    expect_equal(
        formatter.format("FDX606"),
        "Foxtrot Delta X-ray Six Zero Six",
        "callsign FDX606");

    expect_equal(
        formatter.format("ABC123"),
        "Alpha Bravo Charlie One Two Three",
        "callsign ABC123");

    expect_equal(
        formatter.format("Runway 18R"),
        "Runway One Eight Right",
        "runway 18R");

    expect_equal(
        formatter.format("Runway 4L"),
        "Runway Four Left",
        "runway 4L");

    expect_equal(
        formatter.format(
            "FDX606, hold short of Runway 18R."),
        "Foxtrot Delta X-ray Six Zero Six, hold short of Runway One Eight Right.",
        "combined callsign and runway");

    expect_equal(
        formatter.format(
            "Maintain current state and request controller review."),
        "Maintain current state and request controller review.",
        "ordinary prose unchanged");

    return 0;
}
