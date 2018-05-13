#pragma once

#include "mail_parser.hpp"
#include "utils.hpp"

class MailPersister : public NonCopyable {
  public:
    MailPersister() = delete;

    static bool store_mail(const Mail& mail);
};
