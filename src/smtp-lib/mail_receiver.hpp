#pragma once

#include <memory>

#include "connection.hpp"
#include "mail_builder.hpp"
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
    MailBuilder m_mail_builder;

	void handle_request(const ParserRequest& request);

	
    SMTPResponse handle_accepted_command(const SMTPCommand& command);
    SMTPResponse handle_complete_command(const SMTPCommand& command);

    void on_mail_finished();
    void on_mail_reset();
    void on_content_start();

    void send_response(const SMTPResponse& response);

    bool no_stop_needed();

    static SMTPResponse get_welcome_response();
    static SMTPResponse get_accepted_response(const SMTPCommandType& type);
    static SMTPResponse get_error_response(const SMTPCommandType& type);
    static SMTPResponse get_error_response(const std::exception& e);
    static SMTPResponse get_error_response();
    static SMTPResponse get_error_response(const ParserStatus type);
};
