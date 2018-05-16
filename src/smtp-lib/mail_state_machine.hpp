#pragma once

#include <smtp_helper.hpp>


class MailStateMachine {
  public:
    MailStateMachine();

    SMTPState current_state() const;
    SimplifiedSMTPState current_simplified_state() const;
    SMTPResponse accept(const SMTPCommand& command);

  private:
    void handle_command(const SMTPCommand& command);
    void advance_state(const SMTPCommand& command);
    SMTPResponse create_valid_response(const SMTPCommand& command);
    SMTPResponse create_invalid_response();

    SMTPState m_state;
};
