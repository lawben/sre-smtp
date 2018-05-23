#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/envelop_parser.hpp"

TEST_CASE("test HELO", "[unit][envelop_parser]") {
    EnvelopParser parser;
    const auto responses = parser.accept(ParserRequest("HELO my-sender\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::HELO);
    CHECK(responses[0].data == "my-sender");
}

TEST_CASE("test MAIL", "[unit][envelop_parser]") {
    EnvelopParser parser;
    const auto responses = parser.accept(ParserRequest("MAIL FROM:reverse-path\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::MAIL);
    CHECK(responses[0].data == "reverse-path");
}

TEST_CASE("test RCPT", "[unit][envelop_parser]") {
    EnvelopParser parser;
    const auto responses = parser.accept(ParserRequest("RCPT TO:my-rec\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::RCPT);
    CHECK(responses[0].data == "my-rec");
}

TEST_CASE("test DATA_BEGIN", "[unit][envelop_parser]") {
    EnvelopParser parser;
    const auto responses = parser.accept(ParserRequest("DATA\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::DATA_BEGIN);
    CHECK(responses[0].data == "");
}

TEST_CASE("test splited command", "[unit][envelop_parser]") {
    EnvelopParser parser;
    auto responses = parser.accept(ParserRequest("HELO my-send"));
    REQUIRE(responses.size() == 0);
	
	responses = parser.accept(ParserRequest("er\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::HELO);
    CHECK(responses[0].data == "my-sender");
}

TEST_CASE("test invalid message", "[unit][envelop_parser]") {
    EnvelopParser parser;
    auto responses = parser.accept(ParserRequest("WRONGCMD\r\n"));
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::INVALID);
    CHECK(responses[0].data == "WRONGCMD");
}

TEST_CASE("test incomplete line", "[unit][envelop_parser]") {
    EnvelopParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("my messages"));
    CHECK(responses.empty());
    responses = parser.accept(ParserRequest("are the"));
    CHECK(responses.empty());
    responses = parser.accept(ParserRequest("very best!"));
    CHECK(responses.empty());
}

TEST_CASE("case sensitive identifiers", "[unit][envelop_parser]") {
    EnvelopParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("rcpT To:my-sender\r\n"));

    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::RCPT);
    CHECK(responses[0].data == "my-sender");
}