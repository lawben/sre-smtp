#pragma once

#include "smtp_helper.hpp"


class MailStateMachine {
  public:
    MailStateMachine();

    SMTPState current_state() const;
    SimplifiedSMTPState current_simplified_state() const;
    SMTPResponse accept(const SMTPCommand& command);

  private:
    bool is_valid_command(const SMTPCommand& command);
    void handle_command(const SMTPCommand& command);
    SMTPState advanced_state(const SMTPCommand& command);
    SMTPResponse create_valid_response(const SMTPCommand& command);
    SMTPResponse create_invalid_response();

    SMTPState m_state;
};
