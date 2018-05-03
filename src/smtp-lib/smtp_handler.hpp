#pragma once

#include "connection.hpp"
#include "mail_parser.hpp"

class SMTPHandler : public NonCopyable {
 public:
  explicit SMTPHandler(std::unique_ptr<Connection> connection);

  // This is the main SMTP loop. This assumes a valid connection with read/write. It will run in a separate thread.
  void run();

 private:
  std::unique_ptr<Connection> m_connection;
  MailParser m_parser{};

  bool m_error_occurred = false;
};


