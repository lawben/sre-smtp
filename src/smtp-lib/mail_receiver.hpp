#pragma once

#include "connection.hpp"
#include "mail_parser.hpp"
#include "mail_state_machine.hpp"

class MailReceiver : public NonCopyable {
  public:
    explicit MailReceiver(std::unique_ptr<Connection> connection);

    // This is the main SMTP loop. This assumes a valid connection with read/write. It will run in a separate thread.
    void run();

  private:
    std::unique_ptr<Connection> m_connection;
    MailParser m_parser;
    MailStateMachine m_state_machine;

    void send_response(const std::string& msg);

    bool m_error_occurred = false;
};
