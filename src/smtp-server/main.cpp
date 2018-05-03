#include <iostream>

#include "smtp-lib/smtp_server.hpp"

int main() {
  try {
    SMTPServer::run();
  } catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
    return 1;
  }
}