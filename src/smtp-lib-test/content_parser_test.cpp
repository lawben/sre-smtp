#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/content_parser.hpp"


TEST_CASE("test DATA body", "[unit][content_parser]") {
    ContentParser parser;

    auto responses = parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\n"));
    
	REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::DATA);
    CHECK(responses[0].data == "line-a\r\nline-b\r\nline-c");
}


TEST_CASE("multiple lines", "[unit][content_parser]") {
    ContentParser parser;
    
	auto responses = parser.accept(ParserRequest("This "));

    CHECK(responses.size() == 0);

    responses = parser.accept(ParserRequest("is "));

	CHECK(responses.size() == 0);

    responses = parser.accept(ParserRequest("Testing!!!?"));

	CHECK(responses.size() == 0);

    responses = parser.accept(ParserRequest("\r\n.\r\n"));

    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::DATA);
    CHECK(responses[0].data == "This is Testing!!!?");
}