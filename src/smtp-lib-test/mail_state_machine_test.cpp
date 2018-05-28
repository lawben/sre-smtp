#include "catch/catch.hpp"

#include "smtp-lib/mail_state_machine.hpp"

TEST_CASE("basic smtp protocoll", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    CHECK(state_machine.current_state() == SMTPState::CLIENT_INIT);

    state_machine.accept(SMTPCommandType::HELO);
    CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);

    state_machine.accept(SMTPCommandType::MAIL);
    CHECK(state_machine.current_state() == SMTPState::RCPT_TO);

    state_machine.accept(SMTPCommandType::RCPT);
    CHECK(state_machine.current_state() == SMTPState::RCPT_TO_OR_DATA_BEGIN);

    state_machine.accept(SMTPCommandType::DATA_BEGIN);
    CHECK(state_machine.current_state() == SMTPState::DATA_CONTENT);

    state_machine.accept(SMTPCommandType::DATA);
    CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
}

TEST_CASE("check state CLIENT INIT", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    CHECK(state_machine.current_state() == SMTPState::CLIENT_INIT);

    SECTION("helo") { CHECK(state_machine.accept(SMTPCommandType::HELO)); }
    SECTION("reset") { CHECK(state_machine.accept(SMTPCommandType::RSET)); }
    SECTION("quit") { CHECK(state_machine.accept(SMTPCommandType::QUIT)); }

    SECTION("from") { CHECK_FALSE(state_machine.accept(SMTPCommandType::MAIL)); }
    SECTION("to") { CHECK_FALSE(state_machine.accept(SMTPCommandType::RCPT)); }
    SECTION("data") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA)); }
    SECTION("data begin") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA_BEGIN)); }
    SECTION("data begin") { CHECK_FALSE(state_machine.accept(SMTPCommandType::INVALID)); }
}

TEST_CASE("check state MAIL FROM", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    state_machine.accept(SMTPCommandType::HELO);
    CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);

    SECTION("reset") { CHECK(state_machine.accept(SMTPCommandType::RSET)); }
    SECTION("quit") { CHECK(state_machine.accept(SMTPCommandType::QUIT)); }
    SECTION("from") { CHECK(state_machine.accept(SMTPCommandType::MAIL)); }
    SECTION("helo") { CHECK(state_machine.accept(SMTPCommandType::HELO)); }

    SECTION("to") { CHECK_FALSE(state_machine.accept(SMTPCommandType::RCPT)); }
    SECTION("data") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA)); }
    SECTION("data begin") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA_BEGIN)); }
    SECTION("invalid") { CHECK_FALSE(state_machine.accept(SMTPCommandType::INVALID)); }
}

TEST_CASE("check state RCPT TO", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    state_machine.accept(SMTPCommandType::HELO);
    state_machine.accept(SMTPCommandType::MAIL);
    CHECK(state_machine.current_state() == SMTPState::RCPT_TO);

    SECTION("reset") { CHECK(state_machine.accept(SMTPCommandType::RSET)); }
    SECTION("quit") { CHECK(state_machine.accept(SMTPCommandType::QUIT)); }
    SECTION("to") { CHECK(state_machine.accept(SMTPCommandType::RCPT)); }
    SECTION("helo") { CHECK(state_machine.accept(SMTPCommandType::HELO)); }

    SECTION("from") { CHECK_FALSE(state_machine.accept(SMTPCommandType::MAIL)); }
    SECTION("data") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA)); }
    SECTION("data begin") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA_BEGIN)); }
    SECTION("invalid") { CHECK_FALSE(state_machine.accept(SMTPCommandType::INVALID)); }
}

TEST_CASE("check state RCPT TO OR DATA BEGIN", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    state_machine.accept(SMTPCommandType::HELO);
    state_machine.accept(SMTPCommandType::MAIL);
    state_machine.accept(SMTPCommandType::RCPT);
    CHECK(state_machine.current_state() == SMTPState::RCPT_TO_OR_DATA_BEGIN);

    SECTION("reset") { CHECK(state_machine.accept(SMTPCommandType::RSET)); }
    SECTION("quit") { CHECK(state_machine.accept(SMTPCommandType::QUIT)); }
    SECTION("to") { CHECK(state_machine.accept(SMTPCommandType::RCPT)); }
    SECTION("data begin") { CHECK(state_machine.accept(SMTPCommandType::DATA_BEGIN)); }
    SECTION("helo") { CHECK(state_machine.accept(SMTPCommandType::HELO)); }

    SECTION("from") { CHECK_FALSE(state_machine.accept(SMTPCommandType::MAIL)); }
    SECTION("data") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA)); }
    SECTION("invalid") { CHECK_FALSE(state_machine.accept(SMTPCommandType::INVALID)); }
}

TEST_CASE("check state DATA CONTENT", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;
    state_machine.accept(SMTPCommandType::HELO);
    state_machine.accept(SMTPCommandType::MAIL);
    state_machine.accept(SMTPCommandType::RCPT);
    state_machine.accept(SMTPCommandType::DATA_BEGIN);
    CHECK(state_machine.current_state() == SMTPState::DATA_CONTENT);

    SECTION("data") { CHECK(state_machine.accept(SMTPCommandType::DATA)); }

    SECTION("reset") { CHECK_FALSE(state_machine.accept(SMTPCommandType::RSET)); }
    SECTION("quit") { CHECK_FALSE(state_machine.accept(SMTPCommandType::QUIT)); }
    SECTION("to") { CHECK_FALSE(state_machine.accept(SMTPCommandType::RCPT)); }
    SECTION("data begin") { CHECK_FALSE(state_machine.accept(SMTPCommandType::DATA_BEGIN)); }
    SECTION("from") { CHECK_FALSE(state_machine.accept(SMTPCommandType::MAIL)); }
    SECTION("helo") { CHECK_FALSE(state_machine.accept(SMTPCommandType::HELO)); }
    SECTION("invalid") { CHECK_FALSE(state_machine.accept(SMTPCommandType::INVALID)); }
}

TEST_CASE("reset transmission", "[unit][mail_state_machine]") {
    MailStateMachine state_machine;

    SECTION("MAIL FROM") {
        state_machine.accept(SMTPCommandType::HELO);

        SECTION("reset transmission") {
            state_machine.accept(SMTPCommandType::RSET);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }

        SECTION("resent helo") {
            state_machine.accept(SMTPCommandType::HELO);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }
    }

    SECTION("RCPT TO") {
        state_machine.accept(SMTPCommandType::HELO);
        state_machine.accept(SMTPCommandType::MAIL);

        SECTION("reset transmission") {
            state_machine.accept(SMTPCommandType::RSET);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }

        SECTION("resent helo") {
            state_machine.accept(SMTPCommandType::HELO);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }
    }

    SECTION("RCPT TO OR DATA BEGIN") {
        state_machine.accept(SMTPCommandType::HELO);
        state_machine.accept(SMTPCommandType::MAIL);
        state_machine.accept(SMTPCommandType::RCPT);

        SECTION("reset transmission") {
            state_machine.accept(SMTPCommandType::RSET);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }
        SECTION("resent helo") {
            state_machine.accept(SMTPCommandType::HELO);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }
    }

    SECTION("DATA CONTENT") {
        state_machine.accept(SMTPCommandType::HELO);
        state_machine.accept(SMTPCommandType::MAIL);
        state_machine.accept(SMTPCommandType::RCPT);
        state_machine.accept(SMTPCommandType::DATA_BEGIN);

        SECTION("start new transmission") {
            state_machine.accept(SMTPCommandType::DATA);
            CHECK(state_machine.current_state() == SMTPState::MAIL_FROM);
        }
    }
}
