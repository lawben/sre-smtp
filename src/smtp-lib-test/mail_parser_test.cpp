#include <iostream>
#include "catch/catch.hpp"

#include "smtp-lib/mail_parser.hpp"

TEST_CASE("test DATA body", "[unit][mail_parser]") {
    auto parser = MailParser::get_content_parser();

    REQUIRE(parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::DATA);
    CHECK(command.data == "line-a\r\nline-b\r\nline-c");
}

TEST_CASE("multiple lines", "[unit][mail_parser]") {
    auto parser = MailParser::get_content_parser();

    CHECK(parser.accept(ParserRequest("This ")) == ParserStatus::INCOMPLETE);
    CHECK(parser.accept(ParserRequest("is ")) == ParserStatus::INCOMPLETE);
    CHECK(parser.accept(ParserRequest("Testing!!!?")) == ParserStatus::INCOMPLETE);

    CHECK(parser.accept(ParserRequest("\r\n.\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::DATA);
    CHECK(command.data == "This is Testing!!!?");
}

TEST_CASE("test message extends end data token", "[unit][mail_parser]") {
    auto parser = MailParser::get_content_parser();

    REQUIRE(parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\nThisShouldNotBeHere")) ==
            ParserStatus::TO_LONG);
    REQUIRE(parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\nT")) == ParserStatus::TO_LONG);
    REQUIRE(parser.accept(ParserRequest("line-a\r\nline-b\r\nline-c\r\n.\r\n\r\n.\r\n")) == ParserStatus::TO_LONG);
}

TEST_CASE("test HELO", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("HELO my-sender\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::HELO);
    CHECK(command.data == "my-sender");
}

TEST_CASE("test MAIL", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("MAIL FROM:reverse-path\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::MAIL);
    CHECK(command.data == "reverse-path");
}

TEST_CASE("test RCPT", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("RCPT TO:my-rec\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::RCPT);
    CHECK(command.data == "my-rec");
}

TEST_CASE("test DATA_BEGIN", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();
    REQUIRE(parser.accept(ParserRequest("DATA\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::DATA_BEGIN);
    CHECK(command.data == "");
}

TEST_CASE("test splited command", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();
    REQUIRE(parser.accept(ParserRequest("HELO my-send")) == ParserStatus::INCOMPLETE);
    REQUIRE(parser.accept(ParserRequest("er\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::HELO);
    CHECK(command.data == "my-sender");
}

TEST_CASE("test invalid message", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("WRONGCMD\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::INVALID);
    CHECK(command.data == "WRONGCMD");
}

TEST_CASE("test message extends end token", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("RCPT TO:my-rec\r\n\r\nThisShouldNotBeHere")) == ParserStatus::TO_LONG);
    REQUIRE(parser.accept(ParserRequest("RCPT TO:my-rec\r\n\r\n\r\n\r\n")) == ParserStatus::TO_LONG);
    REQUIRE(parser.accept(ParserRequest("RCPT TO:my-rec\r\n\r\nT")) == ParserStatus::TO_LONG);
}

TEST_CASE("test incomplete line", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("my messages")) == ParserStatus::INCOMPLETE);
    REQUIRE(parser.accept(ParserRequest("are the")) == ParserStatus::INCOMPLETE);
    REQUIRE(parser.accept(ParserRequest("very best!")) == ParserStatus::INCOMPLETE);
}

TEST_CASE("case sensitive identifiers", "[unit][mail_parser]") {
    auto parser = MailParser::get_envelop_parser();

    REQUIRE(parser.accept(ParserRequest("rcpT To:my-sender\r\n")) == ParserStatus::COMPLETE);

    const auto command = parser.get_command();
    CHECK(command.type == SMTPCommandType::RCPT);
    CHECK(command.data == "my-sender");
}
