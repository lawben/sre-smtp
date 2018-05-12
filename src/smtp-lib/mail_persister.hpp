#pragma once

#include "utils.hpp"
#include "mail_parser.hpp"

class MailPersister : public NonCopyable {
  public:
    MailPersister() = delete;

    static bool store_mail(const Mail& mail);
};


