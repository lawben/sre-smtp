#pragma once

#include "connection.hpp"
#include "mail_parser.hpp"

class MailReceiver {
  public:
    explicit MailReceiver(Connection connection);
	MailReceiver(MailReceiver &&) = default;
	MailReceiver& operator=(MailReceiver&&) = default;

    void run();

	void stop();

  private:
    Connection m_connection;
	bool m_stop_requested = false;
    bool m_error_occurred = false;
    MailParser m_parser;

	std::string handle_command(const SMTPCommand& commnad);
	void send_response(const std::string& msg);

	bool NoStopNeeded();
};
