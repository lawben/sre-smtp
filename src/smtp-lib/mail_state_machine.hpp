#pragma once

#include "smtp_utils.hpp"

class MailStateMachine {
  public:
    MailStateMachine();

    SMTPState current_state() const;
    bool accept(const SMTPCommandType& type);

  private:
    SMTPState m_state;

    bool is_valid_command(const SMTPCommandType& type);
    void advanced_state(const SMTPCommandType& type);
};
