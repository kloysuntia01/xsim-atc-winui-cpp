#include "pch.h"

#include "../xSimAtc.Speech/atc_command_parser.h"
#include "../xSimAtc.Speech/atc_readback_formatter.h"

#include <gtest/gtest.h>

namespace
{
    using xsimatc::speech::AtcAction;
    using xsimatc::speech::AtcCommand;
    using xsimatc::speech::AtcCommandParser;
    using xsimatc::speech::AtcReadbackFormatter;
}

TEST(
    AtcCommandParserTests,
    ParsesCompactFedExCallSign)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to land");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(command->call_sign, "FDX606");
}

TEST(
    AtcCommandParserTests,
    ParsesSpokenFedExCallSign)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fedex six zero six cleared to land");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(command->call_sign, "FDX606");
}

TEST(
    AtcCommandParserTests,
    ParsesClearedToLandAction)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to land");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(
        command->action,
        AtcAction::ClearedToLand);
}

TEST(
    AtcCommandParserTests,
    RejectsUnsupportedAction)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 taxi to runway one eight left");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcCommandParserTests,
    RejectsMissingCallSign)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "cleared to land");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcCommandParserTests,
    ParsesCompactClearedForTakeoff)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared for takeoff");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(command->call_sign, "FDX606");
    EXPECT_EQ(
        command->action,
        AtcAction::ClearedForTakeoff);
}

TEST(
    AtcCommandParserTests,
    ParsesSpokenClearedForTakeoff)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fedex six zero six cleared for takeoff");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(command->call_sign, "FDX606");
    EXPECT_EQ(
        command->action,
        AtcAction::ClearedForTakeoff);
}

TEST(
    AtcCommandParserTests,
    RejectsIncompleteTakeoffPhrase)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared for");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcCommandParserTests,
    RejectsIncorrectTakeoffPhrase)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to takeoff");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcCommandParserTests,
    ParsesSpokenLandingRunway)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fedex six zero six cleared to land runway one eight left");

    ASSERT_TRUE(command.has_value());
    ASSERT_TRUE(command->runway.has_value());
    EXPECT_EQ(*command->runway, "18L");
}

TEST(
    AtcCommandParserTests,
    ParsesSpokenTakeoffRunway)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fedex six zero six cleared for takeoff runway one eight right");

    ASSERT_TRUE(command.has_value());
    ASSERT_TRUE(command->runway.has_value());
    EXPECT_EQ(*command->runway, "18R");
}

TEST(
    AtcCommandParserTests,
    ParsesNumericRunway)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to land runway 18 left");

    ASSERT_TRUE(command.has_value());
    ASSERT_TRUE(command->runway.has_value());
    EXPECT_EQ(*command->runway, "18L");
}

TEST(
    AtcCommandParserTests,
    ClearanceWithoutRunwayRemainsValid)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared for takeoff");

    ASSERT_TRUE(command.has_value());
    EXPECT_FALSE(command->runway.has_value());
}

TEST(
    AtcCommandParserTests,
    RejectsIncompleteRunway)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to land runway");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcCommandParserTests,
    RejectsUnsupportedRunwayDesignator)
{
    const AtcCommandParser parser;

    const auto command =
        parser.parse(
            "fdx606 cleared to land runway one eight alpha");

    EXPECT_FALSE(command.has_value());
}

TEST(
    AtcReadbackFormatterTests,
    FormatsLandingWithoutRunway)
{
    const AtcReadbackFormatter formatter;

    const AtcCommand command{
        .call_sign = "FDX606",
        .action = AtcAction::ClearedToLand
    };

    const auto readback =
        formatter.format(command);

    ASSERT_TRUE(readback.has_value());
    EXPECT_EQ(
        *readback,
        "FedEx six zero six, cleared to land.");
}

TEST(
    AtcReadbackFormatterTests,
    FormatsLandingWithRunway)
{
    const AtcReadbackFormatter formatter;

    const AtcCommand command{
        .call_sign = "FDX606",
        .action = AtcAction::ClearedToLand,
        .runway = "18L"
    };

    const auto readback =
        formatter.format(command);

    ASSERT_TRUE(readback.has_value());
    EXPECT_EQ(
        *readback,
        "FedEx six zero six, cleared to land runway one eight left.");
}

TEST(
    AtcReadbackFormatterTests,
    FormatsTakeoffWithRunway)
{
    const AtcReadbackFormatter formatter;

    const AtcCommand command{
        .call_sign = "FDX606",
        .action = AtcAction::ClearedForTakeoff,
        .runway = "18R"
    };

    const auto readback =
        formatter.format(command);

    ASSERT_TRUE(readback.has_value());
    EXPECT_EQ(
        *readback,
        "FedEx six zero six, cleared for takeoff runway one eight right.");
}

TEST(
    AtcReadbackFormatterTests,
    FormatsCenterRunway)
{
    const AtcReadbackFormatter formatter;

    const AtcCommand command{
        .call_sign = "FDX606",
        .action = AtcAction::ClearedToLand,
        .runway = "18C"
    };

    const auto readback =
        formatter.format(command);

    ASSERT_TRUE(readback.has_value());
    EXPECT_EQ(
        *readback,
        "FedEx six zero six, cleared to land runway one eight center.");
}

TEST(
    AtcReadbackFormatterTests,
    RejectsUnknownAction)
{
    const AtcReadbackFormatter formatter;

    const AtcCommand command{
        .call_sign = "FDX606",
        .action = AtcAction::Unknown
    };

    EXPECT_FALSE(
        formatter.format(command).has_value());
}
