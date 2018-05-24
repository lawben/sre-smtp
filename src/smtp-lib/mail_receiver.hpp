#pragma once

#include <memory>

#include "mail_parser.hpp"
#include "connection.hpp"
#include "mail_builder.hpp"
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
    MailBuilder m_mail_builder;

	SMTPResponse get_welcome_response() const;
	SMTPResponse get_accepted_response(const SMTPCommandType& type) const;
    SMTPResponse get_not_accepted_response(const SMTPCommandType& type) const;
    SMTPResponse get_error_response(const std::exception& e) const;

    void send_response(const SMTPResponse& response);

    bool no_stop_needed();
};
