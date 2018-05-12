#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/mail_parser.hpp"

TEST_CASE("Test incomplete line", "[MailParser]") {
    MailParser parser;
    std::vector<ParserResponse> responses;
    responses = parser.accept(ParserRequest("my messages"));
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("are the"));
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("very best!"));
    REQUIRE(responses.empty());
}

TEST_CASE("Test complete line", "[MailParser]") {
    MailParser parser;
    std::vector<ParserResponse> responses;
    std::string expectedResponse = "my messages are the very best!";
    responses = parser.accept(ParserRequest("my messages "));
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("are the "));
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("very best!\r\n"));
    REQUIRE(responses.size() == 1);
    REQUIRE(responses[0].message == expectedResponse);
}

TEST_CASE("Test multiple lines", "[MailParser]") {
    MailParser parser;
    std::vector<ParserResponse> responses = parser.accept(ParserRequest("msgA\r\nmsgB\r\nmsgC\r\n"));
    REQUIRE(responses.size() == 3);
    REQUIRE(responses[0].message == "msgA");
    REQUIRE(responses[1].message == "msgB");
    REQUIRE(responses[2].message == "msgC");
}

TEST_CASE("Test data field", "[MailParser]") {
    MailParser parser;
    std::vector<ParserResponse> responses = parser.accept(ParserRequest("DATA\r\nlineA\r\nlineB\r\n.\r\n"));
    REQUIRE(responses.size() == 2);
    REQUIRE(responses[0].message == "DATA");
    REQUIRE(responses[1].message == "lineA\r\nlineB");
}
