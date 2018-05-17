#pragma once

#include <atomic>

#include "connection.hpp"
#include "mail_parser.hpp"

class MailReceiver {
  public:
    explicit MailReceiver(std::unique_ptr<Connection> connection);
	MailReceiver(MailReceiver &&) = default;
	MailReceiver& operator=(MailReceiver&&) = default;

    // This is the main SMTP loop. This assumes a valid connection with read/write. It will run in a separate thread.
    void run();

	void stop();

  private:
    std::unique_ptr<Connection> m_connection;
	bool m_stop_requested = false;
    bool m_error_occurred = false;
    MailParser m_parser;

	std::string handle_command(const SMTPCommand& commnad);
	void send_response(const std::string& msg);

	bool NoStopNeeded();
};
