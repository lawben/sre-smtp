#pragma once

#include "utils.hpp"

class SMTPServer : public NonCopyable {
 public:
  static void run(uint16_t port);
};