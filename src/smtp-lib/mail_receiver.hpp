#pragma once

#include "connection.hpp"
#include "mail_parser.hpp"
#include "mail_state_machine.hpp"

class MailReceiver {
  public:
    explicit MailReceiver(Connection connection);
    MailReceiver(MailReceiver&&) = default;
    MailReceiver& operator=(MailReceiver&&) = default;

    void run();

    void stop();

  private:
    Connection m_connection;
    bool m_stop_requested = false;
    bool m_error_occurred = false;
    MailParser m_parser;
    MailStateMachine m_state_machine;

    void send_response(const std::string& msg);

    bool NoStopNeeded();
};
