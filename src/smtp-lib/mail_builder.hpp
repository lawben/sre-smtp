#pragma once

#include "mail.hpp"

#include "smtp_utils.hpp"

class MailBuilder {
  public:
    Mail build();
    void add(SMTPCommand command);
};