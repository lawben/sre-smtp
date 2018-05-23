#pragma once

#include "mail.hpp"

#include "smtp_utils.hpp"

class MailBuilder {
  public:
    Mail build();
    void add(SMTPCommand command);

  private:
    Mail m_mail_under_construction;

	bool is_mail_valid();
};
