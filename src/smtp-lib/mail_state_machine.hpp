#pragma once

#include <functional>

#include "smtp_utils.hpp"

using callback = std::function<void()>;

class MailStateMachine {
  public:
    MailStateMachine();

    SMTPState current_state() const;
    bool accept(const SMTPCommandType& type);

    void set_mail_rest_callback(std::function<void()> callback);
    void set_content_start_callback(std::function<void()> callback);
    void set_mail_finish_callback(std::function<void()> callback);

  private:
    SMTPState m_state;
    callback m_on_mail_reset = nullptr;
    callback m_on_content_start = nullptr;
    callback m_on_mail_finish = nullptr;

    void mail_reset_transition();
    void enter_content_transition();
    void mail_finish_transition();

    bool is_valid_command(const SMTPCommandType& type);
    void advanced_state(const SMTPCommandType& type);
};
