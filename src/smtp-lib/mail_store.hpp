#pragma once

#include "mail.hpp"
class MailStore {
  public:
    MailStore() = delete;

    static bool store_mail(const Mail& mail);
};


