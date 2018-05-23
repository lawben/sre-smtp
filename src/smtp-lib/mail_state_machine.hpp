#pragma once

#include "smtp_utils.hpp"

class MailStateMachine {
  public:
    MailStateMachine();

    SMTPState current_state() const;
    SimplifiedSMTPState current_simplified_state() const;
    SMTPResponse accept(const SMTPCommandType& type);

  private:
    bool is_valid_command(const SMTPCommandType& type);

    SMTPState advanced_state(const SMTPCommandType& type);
    SMTPResponse create_valid_response(const SMTPCommandType& type);
    SMTPResponse create_invalid_response();

    SMTPState m_state;
};
