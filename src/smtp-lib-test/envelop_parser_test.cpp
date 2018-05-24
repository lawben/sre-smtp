#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/envelop_parser.hpp"

TEST_CASE("test HELO", "[unit][envelop_parser]") {
    EnvelopParser parser;
    
	REQUIRE(parser.accept(ParserRequest("HELO my-sender\r\n")) == ParserStatus::COMPLETE);

	const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::HELO);
    CHECK(command.data == "my-sender");
}

TEST_CASE("test MAIL", "[unit][envelop_parser]") {
    EnvelopParser parser;
    
	REQUIRE(parser.accept(ParserRequest("MAIL FROM:reverse-path\r\n")) == ParserStatus::COMPLETE);
    
	const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::MAIL);
    CHECK(command.data == "reverse-path");
}

TEST_CASE("test RCPT", "[unit][envelop_parser]") {
    EnvelopParser parser;
    
	REQUIRE(parser.accept(ParserRequest("RCPT TO:my-rec\r\n")) == ParserStatus::COMPLETE);
    
	const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::RCPT);
    CHECK(command.data == "my-rec");
}

TEST_CASE("test DATA_BEGIN", "[unit][envelop_parser]") {
    EnvelopParser parser;
    REQUIRE(parser.accept(ParserRequest("DATA\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::DATA_BEGIN);
    CHECK(command.data == "");
}

TEST_CASE("test splited command", "[unit][envelop_parser]") {
    EnvelopParser parser;
    REQUIRE(parser.accept(ParserRequest("HELO my-send")) == ParserStatus::INCOMPLETE);
	REQUIRE(parser.accept(ParserRequest("er\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::HELO);
    CHECK(command.data == "my-sender");
}

TEST_CASE("test invalid message", "[unit][envelop_parser]") {
    EnvelopParser parser;

    REQUIRE(parser.accept(ParserRequest("WRONGCMD\r\n")) == ParserStatus::COMPLETE);
    
	const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::INVALID);
    CHECK(command.data == "WRONGCMD");
}

TEST_CASE("test incomplete line", "[unit][envelop_parser]") {
    EnvelopParser parser;

    REQUIRE(parser.accept(ParserRequest("my messages")) == ParserStatus::INCOMPLETE);
    REQUIRE(parser.accept(ParserRequest("are the")) == ParserStatus::INCOMPLETE);
    REQUIRE(parser.accept(ParserRequest("very best!")) == ParserStatus::INCOMPLETE);
}

TEST_CASE("case sensitive identifiers", "[unit][envelop_parser]") {
    EnvelopParser parser;

    REQUIRE(parser.accept(ParserRequest("rcpT To:my-sender\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::RCPT);
    CHECK(command.data == "my-sender");
}