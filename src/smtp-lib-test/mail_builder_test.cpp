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

// invalide token
// multiple from
// multiple data
// reset mail