#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/mail_parser.hpp"

TEST_CASE("test HELO", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("HELO my-sender\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::HELO);
    CHECK(responses[0].data == "my-sender");
}

TEST_CASE("test MAIL", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("MAIL FROM:reverse-path\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::MAIL);
    CHECK(responses[0].data == "reverse-path");
}

TEST_CASE("test RCPT", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("RCPT TO:my-rec\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::MAIL);
    CHECK(responses[0].data == "my-rec");
}

TEST_CASE("test DATA_BEGIN", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("DATA\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::MAIL);
    CHECK(responses[0].data == "");
}

TEST_CASE("test multiple commands", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("HELO my-sender\r\nMAIL FROM:my-sender\r\nRCPT TO:my-rec\r\n"),
                              SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 3);
    CHECK(responses[0].type == SMTPCommandType::HELO);
    CHECK(responses[0].data == "my-sender");
    CHECK(responses[1].type == SMTPCommandType::MAIL);
    CHECK(responses[1].data == "FROM:my-sender");
    CHECK(responses[2].type == SMTPCommandType::RCPT);
    CHECK(responses[2].data == "TO:my-rec");
}

TEST_CASE("test DATA body", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses =
        parser.accept(ParserRequest("DATA\r\nline-a\r\nline-b\r\nline-c\r\n.\r\n"), SimplifiedSMTPState::CONTENT);
    REQUIRE(responses.size() == 2);
    CHECK(responses[0].type == SMTPCommandType::DATA_BEGIN);
    CHECK(responses[1].type == SMTPCommandType::DATA);
    CHECK(responses[1].data == "line-a\r\nline-b\r\nline-c");
}

TEST_CASE("test invalid message", "[unit][mail_parser]") {
    MailParser parser;
    REQUIRE_THROWS_AS(parser.accept(ParserRequest("WRONGCMD\r\n"), SimplifiedSMTPState::ENVELOPE), std::runtime_error);
}

TEST_CASE("test incomplete line", "[mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("my messages"), SimplifiedSMTPState::ENVELOPE);
    CHECK(responses.empty());
    responses = parser.accept(ParserRequest("are the"), SimplifiedSMTPState::ENVELOPE);
    CHECK(responses.empty());
    responses = parser.accept(ParserRequest("very best!"), SimplifiedSMTPState::ENVELOPE);
    CHECK(responses.empty());
}

TEST_CASE("seperated line", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("HELO my-se"), SimplifiedSMTPState::ENVELOPE);
    CHECK(responses.size() == 0);
    responses = parser.accept(ParserRequest("nder\r\n"), SimplifiedSMTPState::ENVELOPE);
    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::HELO);
    CHECK(responses[0].data == "my-sender");
}

TEST_CASE("case sensitive identifiers", "[unit][mail_parser]") {
    MailParser parser;
    std::vector<SMTPCommand> responses;
    responses = parser.accept(ParserRequest("rcpT To:my-sender\r\n"), SimplifiedSMTPState::ENVELOPE);

    REQUIRE(responses.size() == 1);
    CHECK(responses[0].type == SMTPCommandType::RCPT);
    CHECK(responses[0].data == "my-sender");
}