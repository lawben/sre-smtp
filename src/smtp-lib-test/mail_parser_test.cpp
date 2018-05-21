#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/mail_parser.hpp"

TEST_CASE("Test incomplete line", "[MailParser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("my messages"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("are the"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.empty());
    responses = parser.accept(ParserRequest("very best!"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.empty());
}

TEST_CASE("Test HELO", "[MailParser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("HELO my-sender\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    REQUIRE(responses[0].type == SMTPCommandType::HELO);
    REQUIRE(responses[0].data == "my-sender");
}

TEST_CASE("Test MAIL", "[MailParser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("MAIL from:reverse-path\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    REQUIRE(responses[0].type == SMTPCommandType::MAIL);
    REQUIRE(responses[0].data == "from:reverse-path");
}

TEST_CASE("Test multiple commands", "[MailParser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("HELO my-sender\r\nMAIL FROM:my-sender\r\nRCPT TO:my-rec\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 3);
    REQUIRE(responses[0].type == SMTPCommandType::HELO);
    REQUIRE(responses[0].data == "my-sender");
    REQUIRE(responses[1].type == SMTPCommandType::MAIL);
    REQUIRE(responses[1].data == "FROM:my-sender");
    REQUIRE(responses[2].type == SMTPCommandType::RCPT);
    REQUIRE(responses[2].data == "TO:my-rec");
}

TEST_CASE("Test DATA body", "[MailParser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("DATA\r\nline-a\r\nline-b\r\nline-c\r\n.\r\n"), SimplifiedSMTPState::CONTENT);
    REQUIRE(responses.size() == 2);
    REQUIRE(responses[0].type == SMTPCommandType::DATA_BEGIN);
    REQUIRE(responses[1].type == SMTPCommandType::DATA);
    REQUIRE(responses[1].data == "line-a\r\nline-b\r\nline-c");
}

TEST_CASE("Test invalid message", "[MailParser]") {
    MailParser parser;
    REQUIRE_THROWS_AS(parser.accept(ParserRequest("WRONGCMD\r\n"), SimplifiedSMTPState::ENVELOPE), std::runtime_error);
}
