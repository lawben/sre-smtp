#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/content_parser.hpp"

TEST_CASE("test DATA body", "[unit][content_parser]") {
    ContentParser parser;

    REQUIRE(parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::DATA);
    CHECK(command.data == "line-a\r\nline-b\r\nline-c");
}

TEST_CASE("multiple lines", "[unit][content_parser]") {
    ContentParser parser;

    CHECK(parser.accept(ParserRequest("This ")) == ParserStatus::INCOMPLETE);
    CHECK(parser.accept(ParserRequest("is ")) == ParserStatus::INCOMPLETE);
    CHECK(parser.accept(ParserRequest("Testing!!!?")) == ParserStatus::INCOMPLETE);

	CHECK(parser.accept(ParserRequest("\r\n.\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command(); 
    CHECK(command.type == SMTPCommandType::DATA);
    CHECK(command.data == "This is Testing!!!?");
}