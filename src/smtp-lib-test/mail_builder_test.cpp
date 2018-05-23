#include "catch/catch.hpp"

#include "smtp-lib/mail_builder.hpp"
#include "smtp-lib/smtp_utils.hpp"

TEST_CASE("build empty mail", "[unit][mail_builder]") {
    MailBuilder builder;
    REQUIRE_THROWS_AS(builder.build(), std::runtime_error);
}

TEST_CASE("build simple mail", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string from("sender@test.hpi");
    std::string to("receiver@test.hpi");
    std::string data("What's up");

    builder.add({SMTPCommandType::MAIL, from});
    builder.add({SMTPCommandType::RCPT, to});
    builder.add({SMTPCommandType::DATA, data});

    auto mail = builder.build();

    CHECK(mail.from == from);
    CHECK(mail.data == data);

    REQUIRE(mail.to.size() == 1);
    CHECK(mail.to[0] == to);
}

TEST_CASE("use invalide token", "[unit][mail_builder]") {
    MailBuilder builder;
    REQUIRE_THROWS_AS(builder.add({SMTPCommandType::DATA_BEGIN, ""}), std::runtime_error);
}

TEST_CASE("reset mail object", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string from("sender@test.hpi");
    std::string to_1("receiver@test.hpi");
    std::string to_2("receiver@test.hpi");
    std::string data("What's up");

    builder.add({SMTPCommandType::MAIL, from});
    builder.add({SMTPCommandType::RCPT, to_1});
    builder.add({SMTPCommandType::RCPT, to_2});
    builder.add({SMTPCommandType::DATA, data});

    auto mail = builder.build();

    CHECK(mail.from == from);
    CHECK(mail.data == data);

    REQUIRE(mail.to.size() == 2);
    CHECK(mail.to[0] == to_1);
    CHECK(mail.to[1] == to_2);

	REQUIRE_THROWS_AS(builder.build(), std::runtime_error);
}

TEST_CASE("multiple from", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string from("sender@test.hpi");
    builder.add({SMTPCommandType::MAIL, from});

    REQUIRE_THROWS_AS(builder.add({SMTPCommandType::MAIL, from}), std::runtime_error);
}

TEST_CASE("multiple data", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string data("Hello");
    builder.add({SMTPCommandType::DATA, data});

    REQUIRE_THROWS_AS(builder.add({SMTPCommandType::DATA, data}), std::runtime_error);
}

TEST_CASE("reset all", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string data("Hello");
    std::string from("sender@test.hpi");
    std::string to("receiver@test.hpi");

    builder.add({SMTPCommandType::MAIL, "Invalid"});
    builder.add({SMTPCommandType::DATA, "Invalid"});
    builder.add({SMTPCommandType::RCPT, "Invalid"});

    builder.reset();

	builder.add({SMTPCommandType::MAIL, from});
    builder.add({SMTPCommandType::DATA, data});
    builder.add({SMTPCommandType::RCPT, to});

	auto mail = builder.build();

	CHECK(mail.from == from);
    CHECK(mail.data == data);

    REQUIRE(mail.to.size() == 1);
    CHECK(mail.to[0] == to);
}

TEST_CASE("reset parts", "[unit][mail_builder]") {
    MailBuilder builder;

    std::string data("Hello");
    std::string from("sender@test.hpi");
    std::string to("receiver@test.hpi");

    builder.add({SMTPCommandType::MAIL, "Invalid"});
    builder.add({SMTPCommandType::DATA, "Invalid"});
    builder.add({SMTPCommandType::RCPT, "Invalid"});

    builder.reset(SMTPCommandType::MAIL);
    builder.reset(SMTPCommandType::DATA);
    builder.reset(SMTPCommandType::RCPT);

    builder.add({SMTPCommandType::MAIL, from});
    builder.add({SMTPCommandType::DATA, data});
    builder.add({SMTPCommandType::RCPT, to});

    auto mail = builder.build();

    CHECK(mail.from == from);
    CHECK(mail.data == data);

    REQUIRE(mail.to.size() == 1);
    CHECK(mail.to[0] == to);
}